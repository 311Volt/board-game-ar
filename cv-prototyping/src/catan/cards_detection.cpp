#include <catan/cards_detection.hpp>

cv::Mat scaleImage(cv::Mat image, float scale)
{
	cv::Mat scaled;
	cv::resize(image, scaled, cv::Size(image.size().width * scale, image.size().height * scale), cv::INTER_AREA);
	return scaled;
}

cv::Mat detectWhiteFields(cv::Mat image)
{
	cv::Mat hsvCardsPhoto;
	cv::cvtColor(image, hsvCardsPhoto, cv::COLOR_BGR2HSV);

	cv::Mat whiteFieldsofPhoto;
	cv::inRange(hsvCardsPhoto, cv::Vec3b(0, 0, 110), cv::Vec3b(179, 30, 255), whiteFieldsofPhoto);

	return whiteFieldsofPhoto;
}

std::vector<std::vector<cv::Point>> removeSmallContours(std::vector<std::vector<cv::Point>> contours, int contourAreaThreshold)
{
	std::vector<std::vector<cv::Point>> filteredContours;
	for (auto c : contours)
	{
		if (cv::contourArea(c) > contourAreaThreshold)
			filteredContours.push_back(c);
	}
	return filteredContours;
}

std::vector<std::vector<cv::Point>> filterForPolygonsOfNVerticies(std::vector<std::vector<cv::Point>> contours, int n, float periCoefficient)
{
	std::vector<std::vector<cv::Point>> filteredContours;
	for (auto c : contours)
	{
		auto peri = cv::arcLength(c, true);
		std::vector<cv::Point> approx;
		cv::approxPolyDP(c, approx, peri * periCoefficient, true);
		if (approx.size() == n)
			filteredContours.push_back(c);
	}
	return filteredContours;
}

cv::Mat getErodedMask(cv::Mat mask, cv::Vec2b erosionKernel, short erosionIterations)
{
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

	// draw filled white fields - to avoid erosion inside the dark places of cards
	cv::Mat maskEroded = cv::Mat::zeros(mask.size(), CV_8U);
	cv::drawContours(maskEroded, contours, -1, cv::Scalar(255, 255, 255), -1);

	// remove some noises that may have glued to the cards contours
	cv::erode(maskEroded, maskEroded, erosionKernel, cv::Point(-1, -1), erosionIterations);
	return maskEroded;
}

std::vector<std::vector<cv::Point>> filterCardsContours(cv::Mat mask)
{
	//filter contours
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
	std::vector<std::vector<cv::Point>> countoursWithoutNoise = removeSmallContours(contours, 5000);
	std::vector<std::vector<cv::Point>> contoursQuadrangles = filterForPolygonsOfNVerticies(countoursWithoutNoise, 4, 0.04);
	return contoursQuadrangles;
}

/*cv::Mat correctQuadranglePerspective(std::vector<cv::Point> c, cv::Mat image)
{
	std::vector<cv::Point2f> dstPoints = { cv::Point2f(0,0), cv::Point2f(300, 0), cv::Point2f(0,500), cv::Point2f(300,500) };
	auto peri = cv::arcLength(c, true);
	std::vector<cv::Point2f> approx;
	cv::approxPolyDP(c, approx, peri * 0.04, true); //0.025


	cv::Mat perspectiveMat = cv::getPerspectiveTransform(approx, dstPoints);
	cv::Mat warped;
	cv::warpPerspective(image, warped, perspectiveMat, cv::Size(300, 500));
	cv::imshow("Warped", warped);
	cv::waitKey();
	return warped;
}*/

cv::Mat getDilatedMask(std::vector<cv::Point> c, cv::Size imageSize, cv::Vec2b erosionKernel, short erosionIterations)
{
	cv::Mat maskDilatedBack = cv::Mat::zeros(imageSize, CV_8U);
	std::vector<std::vector<cv::Point>> cs = { c };
	cv::drawContours(maskDilatedBack, cs, -1, cv::Scalar(255, 255, 255), -1);
	cv::drawContours(maskDilatedBack, cs, -1, cv::Scalar(255, 255, 255), int(erosionIterations * 1.5));
	return maskDilatedBack;
}

cv::Mat cutOutCard(cv::Mat image, cv::Mat mask)
{
	cv::Mat card;
	cv::bitwise_and(image, image, card, mask);
	std::vector<std::vector<cv::Point>> cs;
	cv::findContours(mask, cs, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
	cv::Rect cardBoundaries = cv::boundingRect(cs[0]);
	card = card(cardBoundaries);
	return card;
}

std::vector<cv::Mat> detectCards(cv::Mat image)
{
	//parameters of erosion
	cv::Vec2b erosionKernel = cv::Vec2b(5, 5);
	short erosionIterations = 8; //4

	cv::Mat whiteFieldsOfPhoto = detectWhiteFields(image);
	cv::Mat maskEroded = getErodedMask(whiteFieldsOfPhoto, erosionKernel, erosionIterations);
	std::vector<std::vector<cv::Point>> filteredContours = filterCardsContours(maskEroded);

	std::vector<cv::Mat> detectedCards;
	for (auto c : filteredContours)
	{
		cv::Mat maskDilatedBack = getDilatedMask(c, image.size(), erosionKernel, erosionIterations);
		cv::Mat card = cutOutCard(image, maskDilatedBack);
		detectedCards.push_back(card);
	}

	return detectedCards;
}