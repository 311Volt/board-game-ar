
#include <opencv2/opencv.hpp>

#include <catan/board_detection.hpp>
#include <catan/analysis.hpp>
#include <catan/utility_opencv.hpp>
#include <catan/image_correction.hpp> //TODO delete this #include

#include <fmt/format.h>


void DrawCellTypes(cv::Mat board, const ctn::BoardInfo& boardInfo)
{
	ctn::ScreenCoordMapper mapper ({.center = {500,433}, .size = 150});

	for(const auto& [coord, type]: boardInfo.cellTypes) {
		auto pos = mapper(coord) - cv::Point2d{40, 0};
		auto color = cv::Scalar{0, 255, 0};
		cv::putText(board, type, pos, cv::FONT_HERSHEY_COMPLEX_SMALL, 0.9, color);
	}
}


void DrawSettlements(cv::Mat board, const ctn::BoardInfo& boardInfo)
{
	ctn::ScreenCoordMapper mapper ({.center = {500,433}, .size = 150});

	for(const auto& [coord, settlement]: boardInfo.settlements) {
		cv::circle(board, mapper(coord), 9, ctn::PlayerColorBGR(settlement.color), -1);
		cv::putText(
			board, 
			(settlement.type == ctn::SettlementType::City ? "city" : "settlement"), 
			mapper(coord), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, 
			{255,255,255}
		);
	}
}

void DrawRoads(cv::Mat board, const ctn::BoardInfo& boardInfo)
{
	ctn::ScreenCoordMapper mapper ({.center = {500,433}, .size = 150});

	for(const auto& [coord, road]: boardInfo.roads) {
		cv::circle(board, mapper(coord), 7, {255,255,255}, -1);
		cv::circle(board, mapper(coord), 4, ctn::PlayerColorBGR(road.color), -1);
	}
}

double GetTime()
{
	static auto t0 = std::chrono::high_resolution_clock::now();
	auto t1 = std::chrono::high_resolution_clock::now();
	return 1e-9 * std::chrono::duration_cast<std::chrono::nanoseconds>(t1-t0).count();
}


//----- cards detection -----//

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



int main()
{
	/*auto src = cv::imread("resources/sample1.jpg");
	CatanBoardDetector detector {SEA_COLOR_YCBCR_6500K};

	auto warpedOpt = detector.findBoard(src);
	if(!warpedOpt.has_value()) {
		std::cerr << "error: board not found\n";
		return 1;
	}
	cv::Mat warped = warpedOpt.value();

	double t0 = GetTime();
	cv::Mat warpedBlurred = NEW_MAT(tmp) {cv::medianBlur(warped, tmp, 3);};
	cv::imshow("mask2", CreateStructureMask(warpedBlurred, {0,0}));
	double t1 = GetTime();
	fmt::print("masking took {:.6f} secs\n", t1-t0);


	cv::Mat warpMtx = FindAlignment(IsolateDarkEdges(warped), GenerateReferenceEdgeThres());
	cv::Mat warped1;
	cv::warpAffine(warped, warped1, warpMtx, warped.size(), cv::INTER_LINEAR + cv::WARP_INVERSE_MAP);
	warped = warped1;



	cv::imshow("warped1", warped1);

	ctn::BoardIR boardIR = ctn::CreateBoardIR(warped);
	
	ctn::BoardInfo boardInfo = ctn::AnalyzeBoard(boardIR);

	DrawCellTypes(warped, boardInfo);
	DrawSettlements(warped, boardInfo);
	DrawRoads(warped, boardInfo);
	
	cv::imshow("Warped board", warped);
	cv::waitKey();*/

	//----- cards detection -----//
	cv::Mat srcCardsPhoto = cv::imread("resources/sampleBoardWithCards.jpg");
	srcCardsPhoto = scaleImage(srcCardsPhoto, 0.3);
	cv::imshow("Src Cards Photo", srcCardsPhoto);

	std::vector<cv::Mat> croppedOutCards = detectCards(srcCardsPhoto);

	// show every single cropped out card
	int i = 1;
	for (auto card : croppedOutCards)
	{
		cv::imshow("Card " + std::to_string(i), card);
		i++;
	}

	std::vector<cv::Mat> templateCards = readTemplateCards();
	// show templates
	int z = 1;
	for (auto temp : templateCards)
	{
		//cv::imshow("Temp " + std::to_string(j), scaleImage(temp, 0.3));
		cv::imshow("Temp " + std::to_string(z), temp);
		z++;
	}

	/*auto orb = cv::ORB::create(); // ORB
	auto matcher = cv::BFMatcher::create();
	std::vector<std::vector<cv::KeyPoint>> keypointsTemplates;
	std::vector<cv::Mat> descriptorsTemplates;

	// prepare orb thingies for all template cards
	int j = 1;
	for (auto temp : templateCards)
	{
		std::vector<cv::KeyPoint> keypointsTemp;
		cv::Mat descriptorsTemp;
		orb->detectAndCompute(temp, NULL, keypointsTemp, descriptorsTemp);
		keypointsTemplates.push_back(keypointsTemp);
		descriptorsTemplates.push_back(descriptorsTemp);
		//cv::imshow("Temp " + std::to_string(j), scaleImage(temp, 0.3));
		cv::imshow("Temp " + std::to_string(j), temp);
		j++;
		
	}

	// trying to do recognize the cards with orby
	// comparing each detected card with all template cards
	int k = 1;
	for (auto card : croppedOutCards)
	{
		cv::Mat grayCard;
		cv::cvtColor(card, grayCard, cv::COLOR_BGR2GRAY);
		std::vector<cv::KeyPoint> keypointsCard;
		cv::Mat descriptorsCard;
		orb->detectAndCompute(grayCard, NULL, keypointsCard, descriptorsCard);
		for (int a = 0; a < templateCards.size(); a++)
		{
			std::vector<cv::DMatch> matches;
			matcher->match(descriptorsTemplates.at(a), descriptorsCard, matches);
			// trying to get the best matches, which means the ones with the least distance
			std::vector<cv::DMatch> goodMatches;
			std::sort(matches.begin(), matches.end(), [](const auto& first, const auto& second) {return first.distance < second.distance; });
			for (int p=0; p<30; p++)
			{
				if(matches.at(p).distance < 300)
					goodMatches.push_back(matches.at(p));
			}
			// saving comparison images with good matches detected to analyze them
			cv::Mat comparisonImage;
			cv::drawMatches(templateCards.at(a), keypointsTemplates.at(a), grayCard, keypointsCard, goodMatches, comparisonImage);
			//cv::imshow("Match " + std::to_string(a), comparisonImage);
			cv::imwrite("resources/test_cards/match_" + std::to_string(k) + ".jpg", comparisonImage);
			k++;
		}
	}*/

	cv::waitKey();



}
