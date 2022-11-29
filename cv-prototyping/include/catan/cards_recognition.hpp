#include <opencv2/opencv.hpp>
#include<tesseract/baseapi.h>
#include<leptonica/allheaders.h>

#include <string>

#include <catan/cards_detection.hpp>
#include <catan/cards_correction.hpp>


/*enum cardType {
	NOT_RECOGNIZED,
	COSTS_TABLE,
	KNIGHT,
	POINT_CATHEDRAL,
	POINT_CITY_HALL,
	POINT_LIBRARY,
	POINT_MARKETPLACE,
	POINT_UNIVERSITY,
	PROGRESS_BUILD_ROADS,
	PROGRESS_INVENTION,
	PROGRESS_MONOPOLY,
	TWO_POINTS_KNIGHTS_POWER,
	TWO_POINTS_LONGEST_ROAD
};*/

enum scoringCardType
{
	OTHER,
	ONE_POINT,
	TWO_POINTS
};

int* recognizeCardsFromImage(cv::Mat image, bool isPlasticVer);