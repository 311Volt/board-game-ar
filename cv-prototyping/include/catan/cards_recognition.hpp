#include <opencv2/opencv.hpp>
#include<tesseract/baseapi.h>
#include<leptonica/allheaders.h>

#include <catan/cards_detection.hpp>
#include <catan/cards_correction.hpp>


int* recognizeCardsFromImage(cv::Mat image, bool isPlasticVer);