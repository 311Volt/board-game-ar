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


std::vector<std::vector<cv::Mat>> cutOutCardHeadings(std::vector<cv::Mat> cards, float headingAreaCoeff)
{
	std::vector<std::vector<cv::Mat>> cardHeadingsPairs;  // beacuse cards may be upside down, there are two possible headers
	for (auto card : cards)
	{
		std::vector<cv::Mat> headingsPair;
		cv::Rect headingBoundingRect = cv::Rect(cv::Point(0, 0), cv::Point(card.size().width - 1, card.size().height * headingAreaCoeff-1));
		cv::Mat heading1 = card(headingBoundingRect);
		headingsPair.push_back(heading1);
		//cv::imshow("Heading 1", heading1);
		//cv::waitKey();

		cv::Mat cardFlippedVertically;
		cv::rotate(card, cardFlippedVertically, cv::ROTATE_180);
		cv::Mat heading2 = cardFlippedVertically(headingBoundingRect);
		headingsPair.push_back(heading2);
		//cv::imshow("Heading 2", heading2);
		//cv::waitKey();

		cardHeadingsPairs.push_back(headingsPair);
	}
	return cardHeadingsPairs;
}