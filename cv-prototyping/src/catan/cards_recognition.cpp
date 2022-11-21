#include <catan/cards_recognition.hpp>


// PS. the cards are downscaled here too
std::vector<cv::Mat> readTemplateCards()
{
	std::vector<cv::Mat> templateCards;
	templateCards.push_back(scaleImage(cv::imread("resources/cards/point_cathedral.jpg", cv::IMREAD_GRAYSCALE), 0.3));
	templateCards.push_back(scaleImage(cv::imread("resources/cards/point_cityHall.jpg", cv::IMREAD_GRAYSCALE), 0.3));
	templateCards.push_back(scaleImage(cv::imread("resources/cards/point_library.jpg", cv::IMREAD_GRAYSCALE), 0.3));
	templateCards.push_back(scaleImage(cv::imread("resources/cards/point_marketplace.jpg", cv::IMREAD_GRAYSCALE), 0.3));
	templateCards.push_back(scaleImage(cv::imread("resources/cards/point_university.jpg", cv::IMREAD_GRAYSCALE), 0.3));
	return templateCards;
}
