#include <catan/cards_recognition.hpp>


/*// PS. the cards are downscaled here too
std::vector<cv::Mat> readTemplateCards()
{
	std::vector<cv::Mat> templateCards;
	templateCards.push_back(scaleImage(cv::imread("resources/cards/point_cathedral.jpg", cv::IMREAD_GRAYSCALE), 0.3));
	templateCards.push_back(scaleImage(cv::imread("resources/cards/point_cityHall.jpg", cv::IMREAD_GRAYSCALE), 0.3));
	templateCards.push_back(scaleImage(cv::imread("resources/cards/point_library.jpg", cv::IMREAD_GRAYSCALE), 0.3));
	templateCards.push_back(scaleImage(cv::imread("resources/cards/point_marketplace.jpg", cv::IMREAD_GRAYSCALE), 0.3));
	templateCards.push_back(scaleImage(cv::imread("resources/cards/point_university.jpg", cv::IMREAD_GRAYSCALE), 0.3));
	return templateCards;
}*/


cv::Mat cutOutCardBottom(cv::Mat card, float bottomAreaCoeff)
{
	cv::Rect bottomBoundingRect = cv::Rect(cv::Point(0, card.size().height * (1.0 - bottomAreaCoeff)), cv::Point(card.size().width - 1, card.size().height - 1));
	cv::Mat cardBottom = card(bottomBoundingRect);
	return cardBottom;
}

cv::Mat cutOutCardHeading(cv::Mat card, float headingAreaCoeff)
{
	cv::Rect headingBoundingRect = cv::Rect(cv::Point(0, 0), cv::Point(card.size().width - 1, card.size().height * headingAreaCoeff - 1));
	cv::Mat cardBottom = card(headingBoundingRect);
	return cardBottom;
}

char* recognizeTextOnImage(cv::Mat cardPart, tesseract::TessBaseAPI* api)
{
	api->SetImage(cardPart.data, cardPart.cols, cardPart.rows, cardPart.channels(), cardPart.step1());
	char* outText = api->GetUTF8Text();
	return outText;
}

int recognizeCard(cv::Mat card, tesseract::TessBaseAPI* api)
{
	cv::Mat cardCopy;
	card.copyTo(cardCopy);
	char* outText = NULL;
	for (int a = 0; a < 2; a++)
	{
		cv::Mat cardBottom = cutOutCardHeading(cardCopy, 0.2);
		outText = recognizeTextOnImage(cardBottom, api);
		std::cout << "OCR output:\n" << outText << std::endl;
		cv::imshow("Card part", cardBottom);
		cv::waitKey();
		cv::rotate(cardCopy, cardCopy, cv::ROTATE_180);
	}

	return 0;
}

int recognizeCardPlasticVer(cv::Mat card, tesseract::TessBaseAPI* api)
{
	cv::Mat cardCopy;
	card.copyTo(cardCopy);
	char* outText = NULL;
	for (int a = 0; a < 2; a++)
	{
		cv::Mat cardBottom = cutOutCardBottom(cardCopy, 0.4);
		outText = recognizeTextOnImage(cardBottom, api);
		std::cout << "OCR output:\n" << outText << std::endl;
		cv::imshow("Card part", cardBottom);
		cv::waitKey();
		cv::rotate(cardCopy, cardCopy, cv::ROTATE_180);
	}
	return 0;
}


int* recognizeCards(std::vector<cv::Mat> cards, bool isPlasticVer)
{
	int* results = NULL;

	tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
	if (api->Init(NULL, "pol")) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
	}

	for (auto card : cards)
	{
		if (!isPlasticVer)
			recognizeCard(card, api);
		else
			recognizeCardPlasticVer(card, api);
	}
	// Destroy used object and release memory
	//api->End();
	//delete api;
	//delete[] outText;
	//pixDestroy(&image);

	api->End();

	return results;
}

int* recognizeCardsFromImage(cv::Mat image, bool isPlasticVer)
{
	int* results;
	std::vector<cv::Mat> croppedOutCards;
	if (!isPlasticVer)
		croppedOutCards = detectCards(image);
	else
		croppedOutCards = detectCardsPlasticVer(image);

	auto correctedCards = correctCardsPerspective(croppedOutCards);
	auto verticalCards = setCardsPositionVertical(correctedCards);

	int j = 1;
	for (auto card : verticalCards)
	{
		cv::imshow("Card warped " + std::to_string(j), card);
		j++;
	}
	cv::waitKey();

	results = recognizeCards(verticalCards, isPlasticVer);
	return results;
}