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
	cv::imshow("hsv", hsvCardsPhoto);

	cv::Mat whiteFieldsofPhoto;
	cv::inRange(hsvCardsPhoto, cv::Vec3b(0, 0, 110), cv::Vec3b(179, 30, 255), whiteFieldsofPhoto);
	cv::imshow("White fields of photo", whiteFieldsofPhoto);

	return whiteFieldsofPhoto;
}

std::vector<std::vector<cv::Point>> filterContoursForBigQuadrangles(std::vector<std::vector<cv::Point>> contours)
{
	std::vector<std::vector<cv::Point>> filteredContours;
	for (auto c : contours)
	{
		if (cv::contourArea(c) > 1000)
		{
			auto peri = cv::arcLength(c, true);
			std::vector<cv::Point> approx;
			cv::approxPolyDP(c, approx, peri * 0.025, true);
			if (approx.size() == 4)
			{
				filteredContours.push_back(c);
			}
		}
	}
	return filteredContours;
}

cv::Mat maskAndCropOutCard(cv::Mat image, std::vector<cv::Point> c)
{
	// masking out single card (or glued, by contours, few cards)
	cv::Mat mask = cv::Mat::zeros(image.size(), image.type());
	std::vector<std::vector<cv::Point>> cs = { c };
	cv::drawContours(mask, cs, -1, cv::Scalar(255, 255, 255), -1);
	cv::bitwise_and(mask, image, mask);

	//cropping out card
	cv::Rect cardBoundaries = cv::boundingRect(c);
	cv::Mat card = mask(cardBoundaries);
	return card;
}

std::vector<cv::Mat> detectCards(cv::Mat image)
{

	cv::Mat whiteFieldsofPhoto = detectWhiteFields(image);
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(whiteFieldsofPhoto, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

	std::vector<std::vector<cv::Point>> filteredContours = filterContoursForBigQuadrangles(contours);

	// show one mask for all cards (for debugging only, to check if all cards where detected)
	cv::Mat maskFiltered = cv::Mat::zeros(image.size(), image.type());
	cv::drawContours(maskFiltered, filteredContours, -1, cv::Scalar(255, 255, 255), -1);
	cv::imshow("Filtered mask", maskFiltered);
	cv::Mat maskedCards;
	cv::bitwise_and(image, maskFiltered, maskedCards);
	cv::imshow("Masked Cards", maskedCards);


	std::vector<cv::Mat> croppedOutCards;
	for (auto c : filteredContours)
	{
		cv::Mat card = maskAndCropOutCard(image, c);
		croppedOutCards.push_back(card);
	}

	return croppedOutCards;
}