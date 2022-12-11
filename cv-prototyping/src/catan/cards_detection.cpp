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
	//cv::inRange(hsvCardsPhoto, cv::Vec3b(0, 0, 110), cv::Vec3b(179, 30, 255), whiteFieldsofPhoto); //original
	cv::inRange(hsvCardsPhoto, cv::Vec3b(0, 0, 110), cv::Vec3b(179, 100, 255), whiteFieldsofPhoto); //Tests3

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


cv::Mat getDilatedMask(std::vector<cv::Point> c, cv::Size imageSize, cv::Vec2b erosionKernel, short erosionIterations)
{
	cv::Mat maskDilatedBack = cv::Mat::zeros(imageSize, CV_8U);
	std::vector<std::vector<cv::Point>> cs = { c };
	cv::drawContours(maskDilatedBack, cs, -1, cv::Scalar(255, 255, 255), -1);
	cv::drawContours(maskDilatedBack, cs, -1, cv::Scalar(255, 255, 255), int(erosionIterations * 1.5));
	//cv::rotate(maskDilatedBack, maskDilatedBack, cv::ROTATE_180);
	//cv::dilate(maskDilatedBack, maskDilatedBack, erosionKernel, cv::Point(-1, -1), erosionIterations*10);
	//cv::rotate(maskDilatedBack, maskDilatedBack, cv::ROTATE_180);

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
	//cv::imshow("Mask eroded", scaleImage(maskEroded, 0.2));
	//cv::waitKey();
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


cv::Mat detectCreamyFields(cv::Mat image)
{
	cv::Mat hsvCardsPhoto;
	cv::cvtColor(image, hsvCardsPhoto, cv::COLOR_BGR2HSV);

	cv::Mat creamyFieldsofPhoto;
	//cv::inRange(hsvCardsPhoto, cv::Vec3b(8, 70, 170), cv::Vec3b(28, 120, 255), creamyFieldsofPhoto); // H = 36/2 For photos with ML
	//cv::inRange(hsvCardsPhoto, cv::Vec3b(12, 70, 150), cv::Vec3b(22, 140, 255), creamyFieldsofPhoto);  // Tests1
	cv::inRange(hsvCardsPhoto, cv::Vec3b(12, 40, 110), cv::Vec3b(28, 180, 255), creamyFieldsofPhoto); //Tests2
	//cv::imshow("Creamy fields", scaleImage(creamyFieldsofPhoto, 0.2));
	//cv::waitKey();

	return creamyFieldsofPhoto;
}

std::vector<cv::Mat> detectCardsPlasticVer(cv::Mat image)
{
	//parameters of erosion
	cv::Vec2b erosionKernel = cv::Vec2b(5, 5);
	short erosionIterations = 8; //4

	cv::Mat creamyFieldsOfPhoto = detectCreamyFields(image);
	cv::Mat maskEroded = getErodedMask(creamyFieldsOfPhoto, erosionKernel, erosionIterations);
	//cv::imshow("Mask eroded", scaleImage(maskEroded, 0.2));
	//cv::waitKey();
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