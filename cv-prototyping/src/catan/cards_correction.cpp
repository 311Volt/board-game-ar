#include <catan/cards_correction.hpp>


std::vector<std::vector<cv::Point>> getMaskedObjectContours(cv::Mat image)
{
	cv::Mat cardGray;
	cv::cvtColor(image, cardGray, cv::COLOR_BGR2GRAY);
	cv::Mat cardThres;
	cv::threshold(cardGray, cardThres, 0, 255, cv::THRESH_BINARY);
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(cardThres, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
	return contours;
}

std::vector<cv::Point> getApproxOfPolygon(std::vector<cv::Point> c, float periCoefficient)
{
	auto peri = cv::arcLength(c, true);
	std::vector<cv::Point> approx;
	cv::approxPolyDP(c, approx, peri * periCoefficient, true);
	return approx;
}

std::vector<cv::Point2f> sortQuadrangleVerticies(std::vector<cv::Point> approx)
{
	cv::Point2f topLeft = approx[0];
	cv::Point2f bottomLeft = approx[1];
	cv::Point2f bottomRight = approx[2];
	cv::Point2f topRight = approx[3];
	std::vector<cv::Point2f> sortedApprox = { topLeft, topRight, bottomRight, bottomLeft };
	return sortedApprox;
}


cv::Size getQuadrangleDimensions(std::vector<cv::Point2f> sortedVerticies)
{
	int width1 = cv::norm(sortedVerticies[0] - sortedVerticies[1]);
	int height1 = cv::norm(sortedVerticies[1] - sortedVerticies[2]);
	int width2 = cv::norm(sortedVerticies[2] - sortedVerticies[3]);
	int height2 = cv::norm(sortedVerticies[3] - sortedVerticies[0]);

	int widthMax = width1 > width2 ? width1 : width2;
	int heightMax = height1 > height2 ? height1 : height2;
	return cv::Size(widthMax, heightMax);
}

std::vector<cv::Point2f> getCorrectionPoints(cv::Size cardDimensions)
{
	std::vector<cv::Point2f> dstPoints;
	cv::Point2f topLeft = cv::Point2f(0, 0);
	cv::Point2f topRight = cv::Point2f(cardDimensions.width - 1, 0);
	cv::Point2f bottomRight = cv::Point2f(cardDimensions.width - 1, cardDimensions.height - 1);
	cv::Point2f bottomLeft = cv::Point2f(0, cardDimensions.height - 1);
	dstPoints = { topLeft, topRight, bottomRight, bottomLeft};
	return dstPoints;
}

std::vector<cv::Mat> correctCardsPerspective(std::vector<cv::Mat> detectedCards)
{
	std::vector<cv::Mat> correctedCards;
	for (auto card : detectedCards)
	{
		std::vector<std::vector<cv::Point>> contours = getMaskedObjectContours(card);
		auto approx = getApproxOfPolygon(contours[0], 0.04);
		if (approx.size() != 4)
			continue;

		std::vector<cv::Point2f> sortedApprox = sortQuadrangleVerticies(approx);
		cv::Mat cardWarped;
		cv::Size dstSize = getQuadrangleDimensions(sortedApprox);
		std::vector<cv::Point2f> dstPoints = getCorrectionPoints(dstSize);
		cv::Mat correctionMatrix = cv::getPerspectiveTransform(sortedApprox, dstPoints);
		cv::warpPerspective(card, cardWarped, correctionMatrix, dstSize);
		correctedCards.push_back(cardWarped);
	}

	return correctedCards;
}

std::vector<cv::Mat> setCardsPositionVertical(std::vector<cv::Mat> cards)
{
	std::vector<cv::Mat> verticalCards;
	int i = 1;
	for (auto card : cards)
	{
		cv::Mat cardVertical;
		if (card.size().width > card.size().height)
			cv::rotate(card, cardVertical, cv::ROTATE_90_CLOCKWISE);
		else
			card.copyTo(cardVertical);
		verticalCards.push_back(cardVertical);
		cv::imwrite("card_corrected_" + std::to_string(i) + ".jpg", card);
		i++;
	}
	return verticalCards;
}