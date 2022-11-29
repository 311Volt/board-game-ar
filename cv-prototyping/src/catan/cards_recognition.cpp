#include <catan/cards_recognition.hpp>


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

std::string trimNonAlphabethical(std::string text)
{
	std::string s = text;
	for (int i = 0; i < s.size(); i++)
	{
		char character = s[i];
		if (!((character >= 'A' && character <= 'Z') || (character >= 'a' && character <= 'z')))
		{
			s.erase(i, 1);
			i--;
		}
	}
	return s;
}

std::string prepareString(std::string text)
{
	std::string s = trimNonAlphabethical(text);
	for (char& character : s)
		character = std::toupper(character);
	return s;
}

scoringCardType assignCardTypeBasedOnText(char* outText)
{
	/*std::string cardsCaptions[] = {"Koszty budowy", "Rycerz", "Katedra", "Ratusz", "Biblioteka", "Rynek", "Uniwersytet", "Postêp",
		"Najwy¿sza W³adza Rycerska 2 Punkty Zwyciêstwa", "Najd³u¿sza Droga Handlowa 2 Punkty Zwyciêstwa" };*/
	std::string twoPointCardsContents[] = { "Najwy¿sza",  "W³adza", "Rycerska", "Najd³u¿sza", "Droga Handlowa" }; // "2 Punkty Zwyciêstwa"
	std::string onePointCardsContents[] = { /*"1",*/  "Punkt", "Zwyciêstwa", "Katedra", "Ratusz", "Biblioteka", "Rynek", "Uniwersytet" };
	std::string cardText = outText;
	cardText = prepareString(cardText);

	std::string costsTableContents[] = { "Koszty budowy", "Rozwój" };
	for (auto templateText : costsTableContents)
	{
		//std::cout << "templateText: " << prepareString(templateText) << std::endl;

		if (cardText.find(prepareString(templateText)) != std::string::npos)
			return scoringCardType::OTHER;
	}

	for (auto templateText : twoPointCardsContents)
	{
		//std::cout << "templateText: " << prepareString(templateText) << std::endl;
		
		if (cardText.find(prepareString(templateText)) != std::string::npos)
			return scoringCardType::TWO_POINTS;
	}
	for (auto templateText : onePointCardsContents)
	{
		//std::cout << "templateText: " << prepareString(templateText) << std::endl;
		if (cardText.find(prepareString(templateText)) != std::string::npos)
			return scoringCardType::ONE_POINT;
	}

	return scoringCardType::OTHER;
}

std::string cardTypeToString(scoringCardType cardType)
{
	switch (cardType)
	{
	case scoringCardType::ONE_POINT:
		return "ONE POINT CARD";
	case scoringCardType::TWO_POINTS:
		return "TWO POINTS CARD";
	default:
		return "OTHER/UNDEFINED CARD";
	}
}

scoringCardType recognizeCard(cv::Mat card, tesseract::TessBaseAPI* api, bool isPlasticVer)
{
	cv::Mat cardCopy;
	card.copyTo(cardCopy);
	char* outText = NULL;
	scoringCardType cardType;
	cv::Mat cardPart;
	for (int a = 0; a < 2; a++)
	{
		if(!isPlasticVer)
			cardPart = cutOutCardHeading(cardCopy, 0.2);
		else
			cardPart = cutOutCardBottom(cardCopy, 0.4);

		outText = recognizeTextOnImage(cardPart, api);
		cardType = assignCardTypeBasedOnText(outText);

		//std::cout << "OCR output:\n" << prepareString(std::string(outText)) << std::endl;
		//std::cout << "Recognition:\n" << cardTypeToString(cardType) << std::endl;
		//cv::imshow("Card part", cardPart);
		//cv::waitKey();

		if (cardType != scoringCardType::OTHER)
			return cardType;
		cv::rotate(cardCopy, cardCopy, cv::ROTATE_180);
	}

	return cardType;
}


std::vector<scoringCardType> recognizeCards(std::vector<cv::Mat> cards, bool isPlasticVer)
{
	std::vector<scoringCardType> results;

	tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
	if (api->Init(NULL, "pol")) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
	}

	for (auto card : cards)
	{
		scoringCardType cardType = recognizeCard(card, api, isPlasticVer);
		results.push_back(cardType);
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
	std::vector<cv::Mat> croppedOutCards;
	if (!isPlasticVer)
		croppedOutCards = detectCards(image);
	else
		croppedOutCards = detectCardsPlasticVer(image);

	auto correctedCards = correctCardsPerspective(croppedOutCards);
	auto verticalCards = setCardsPositionVertical(correctedCards);

	/*int j = 1;
	for (auto card : verticalCards)
	{
		cv::imshow("Card warped " + std::to_string(j), card);
		j++;
	}
	cv::waitKey();*/

	std::vector<scoringCardType> cardTypes = recognizeCards(verticalCards, isPlasticVer);

	for (int i=0; i<verticalCards.size(); i++)
	{
		auto card = verticalCards[i];
		std::string cardTypeString = cardTypeToString(cardTypes[i]);
		cv::putText(card, cardTypeString, cv::Point(card.size().width/4, card.size().height/4), cv::FONT_HERSHEY_COMPLEX_SMALL, 2.0, cv::Scalar(0, 255, 0));
		cv::imshow("Card recognized " + std::to_string(i), card);
	}
	cv::waitKey();

	int* results = new int[cardTypes.size()];
	for (int i=0; i<cardTypes.size(); i++)
		results[i] = (int)cardTypes[i];

	return results;
}