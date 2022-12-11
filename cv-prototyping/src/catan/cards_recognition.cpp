#include <catan/cards_recognition.hpp>
#include <catan/utility_opencv.hpp>
#include <catan/common_math.hpp>


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
	//std::string cardsCaptions[] = {"Koszty budowy", "Rycerz", "Katedra", "Ratusz", "Biblioteka", "Rynek", "Uniwersytet", "Post�p",
		//"Najwy�sza W�adza Rycerska 2 Punkty Zwyci�stwa", "Najd�u�sza Droga Handlowa 2 Punkty Zwyci�stwa" };
	std::string twoPointCardsContents[] = { "Najwy�sza",  "W�adza", "Rycerska", "Najd�u�sza", "Droga Handlowa", "trzy karty rycerz", "pi�� po��czonych dr�g", "Punkty Zwyci�stwa" }; // "2 Punkty Zwyci�stwa"
	std::string onePointCardsContents[] = { /*"1",*/  "Punkt", "Zwyci�stwa", "Katedra", "Ratusz", "Biblioteka", "Rynek", "Uniwersytet" };
	std::string cardText = outText;
	cardText = prepareString(cardText);
	std::cout << cardText << "\n";
	std::string costsTableContents[] = { "Koszty budowy", "Rozw�j", "pkt"};
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

cv::Mat EqualizeHist(cv::Mat input)
{
	auto split = cvutil::SplitBGR(input);
	for(auto& ch: split) {
		cv::equalizeHist(ch, ch);
	}
	return cvutil::MergeBGR(split);
}

double DissimilarityRating(cv::Mat img1, cv::Mat img2)
{
	cv::Mat proc1 = NEW_MAT(tmp) {cv::resize(img1, tmp, {100,150});};
	cv::Mat proc2 = NEW_MAT(tmp) {cv::resize(img2, tmp, {100,150});};
	
	proc1 = EqualizeHist(proc1);
	proc2 = EqualizeHist(proc2);

	proc1 = cvutil::Convert(proc1, cv::COLOR_BGR2YCrCb);
	proc2 = cvutil::Convert(proc2, cv::COLOR_BGR2YCrCb);
	
	//cv::imshow("proc1", proc1);
	//cv::imshow("proc2", proc2);
	//cv::waitKey();

	cv::Mat sqDist = cvmath::TransformBin<cv::Vec3b, float>(proc1, proc2, cvmath::WeightedSquareDist<40, 100, 100>);

	return cv::sum(sqDist)[0];
}

scoringCardType recognizeCard(cv::Mat card, tesseract::TessBaseAPI* api, bool isPlasticVer)
{

	static std::vector<cv::Mat> referenceCards = {
		cv::imread("resources/cards/knight.jpg"),
		cv::imread("resources/cards/point_cathedral.jpg"),
		cv::imread("resources/cards/point_cityHall.jpg"),
		cv::imread("resources/cards/point_library.jpg"),
		cv::imread("resources/cards/point_marketplace.jpg"),
		cv::imread("resources/cards/point_university.jpg"),
		cv::imread("resources/cards/progress_invention.jpg"),
		cv::imread("resources/cards/progress_monopoly.jpg"),
		cv::imread("resources/cards/progress_roads.jpg"),
		cv::imread("resources/cards/build_cost.jpg"),
		cv::imread("resources/cards/longest_road.jpg"),
		cv::imread("resources/cards/triple_knight.jpg"),
		
	};
	static std::vector<std::string> cardNames = {
		"knight", "point_cathedral", "point_cityHall", "point_library",
		"point_marketplace", "point_university", "progress_invention",
		"progress_monopoly", "progress_roads", "build_cost", "longest_road",
		"triple_knight"
	};
	static int indexPoints[] = {
		0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2
	};
	static int counter = 0; 

	std::vector<double> results(referenceCards.size(), 10e9);
	for(int i=0; i<referenceCards.size(); i++) {
		results[i] = DissimilarityRating(card, referenceCards[i]);
		//cv::Mat flipped = NEW_MAT(tmp) {cv::flip(card, tmp, 0);};
		//results[i] = std::min(results[i], DissimilarityRating(flipped, referenceCards[i]));
		std::cout << i << " -> " << results[i] << "\n";
	}
	int minIdx = std::min_element(results.begin(), results.end()) - results.begin();
	std::cout << "__________minIdx = " << minIdx << "\n\n\n\n";

	cv::Mat card1 = card.clone();
	cv::putText(card1, cardNames[minIdx], {10, 30}, cv::FONT_HERSHEY_COMPLEX_SMALL, 0.3, {255,0,0});
	cv::imshow("card" + std::to_string(++counter), card1);

	switch(minIdx) {
		case 1: return scoringCardType::ONE_POINT;
		case 2: return scoringCardType::TWO_POINTS;
		default: return scoringCardType::OTHER;
	}
	/////////////////////////////////////////
	
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

std::vector<cv::Mat> getCardsFromImage(cv::Mat image, bool isPlasticVer)
{
	std::vector<cv::Mat> croppedOutCards;
	if (!isPlasticVer)
		croppedOutCards = detectCards(image);
	else
		croppedOutCards = detectCardsPlasticVer(image);

	auto correctedCards = correctCardsPerspective(croppedOutCards);
	auto verticalCards = setCardsPositionVertical(correctedCards);

	return verticalCards;
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

	//auto verticalCards = getCardsFromImage(image, isPlasticVer);

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
		cv::imshow("Card recognized " + std::to_string(i+1), card);
	}
	cv::waitKey();

	int* results = new int[cardTypes.size()];
	for (int i=0; i<cardTypes.size(); i++)
		results[i] = (int)cardTypes[i];

	return results;
}
