
#include <opencv2/opencv.hpp>

#include <catan/board_detection.hpp>
#include <catan/analysis.hpp>
#include <catan/utility_opencv.hpp>
#include <catan/image_correction.hpp> //TODO delete this #include
#include <catan/cards_detection.hpp>
#include <catan/cards_recognition.hpp>
#include <catan/cards_correction.hpp>

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
	cv::Mat srcCardsPhoto = cv::imread("resources/sampleCards1.jpg");
	//srcCardsPhoto = scaleImage(srcCardsPhoto, 0.3);
	cv::imshow("Src Cards Photo", scaleImage(srcCardsPhoto, 0.3));

	std::vector<cv::Mat> croppedOutCards = detectCards(srcCardsPhoto);

	auto correctedCards = correctCardsPerspective(croppedOutCards);

	int j = 1;
	for (auto card : correctedCards)
	{
		cv::imshow("Card warped " + std::to_string(j), card);
		j++;
	}
	
	// show every single cropped out card
	/*int i = 1;
	for (auto card : croppedOutCards)
	{
		cv::imshow("Card " + std::to_string(i), card);
		i++;
	}*/

	//std::vector<cv::Mat> templateCards = readTemplateCards();

	//attempts to sharpen photo
	/*int i = 1;
	for (auto card : croppedOutCards)
	{
		cv::Mat cardGray;
		cv::Mat cardWithTextOnly;
		cv::cvtColor(card, cardGray, cv::COLOR_BGR2GRAY);
		cv::Mat cardBlurred;
		cv::Mat cardSharpened;
		cv::GaussianBlur(cardGray, cardBlurred, cv::Size(0, 0), cv::BORDER_DEFAULT);
		cv::addWeighted(cardGray, 1.5, cardBlurred, -0.5, 0, cardSharpened);
		//cv::threshold(cardWithTextOnly, cardWithTextOnly, 175, 255, cv::THRESH_OTSU);
		cv::imshow("Card " + std::to_string(i), cardGray);
		i++;
	}*/


	cv::waitKey();

}
