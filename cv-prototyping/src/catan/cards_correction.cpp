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
	std::vector<cv::Point> vectorForSorting = approx;

	// sort descending by sum
	std::sort(vectorForSorting.begin(), vectorForSorting.end(), [](auto a1, auto a2) {return a1.x + a1.y > a2.x + a2.y; });
	cv::Point2f topLeft = vectorForSorting[3];  // the smallest sum
	cv::Point2f bottomRight = vectorForSorting[0]; // the biggest sum

	// sort descending by difference
	std::sort(vectorForSorting.begin(), vectorForSorting.end(), [](auto a1, auto a2) {return a1.x - a1.y > a2.x - a2.y; });
	cv::Point2f topRight = vectorForSorting[0];  // the biggest difference
	cv::Point2f bottomLeft = vectorForSorting[3];  // the smallest difference
	
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

/*cv::Mat setCardPositionVertical(cv::Mat card)
{
	return card;
}*/

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