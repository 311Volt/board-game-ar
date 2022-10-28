#include <iostream>

#include <opencv2/opencv.hpp>

#include <catan/board_coords.hpp>
#include <catan/board_detection.hpp>
#include <catan/image_correction.hpp>
#include <catan/utility_opencv.hpp>
#include <catan/pawns_detection.hpp>

#include <fmt/format.h>




int main()
{
	auto src = cv::imread("resources/sampleGrayBG.jpg");
	CatanBoardDetector detector {SEA_COLOR_YCBCR_6500K};
	cv::Mat warped;
	try {
		warped = detector.findBoard(src).value();
	} catch(std::bad_optional_access& ex) {
		std::cerr << "error: board not found";
		return 1;
	}

	ScreenCoordMapper mapper ({.center = {500,433}, .size = 150});

	/*for (auto& c : GenerateFieldCoords(3)) {
		cv::putText(warped, fmt::format("({},{},{})", c.x, c.y, c.z), mapper(c), cv::FONT_HERSHEY_PLAIN, 1, {255,255,255});
	}

	drawPoints(mapper(GenerateFieldCoords(2)), warped, {255,255,180});
	drawPoints(mapper(GenerateVertexCoords()), warped, {255,0,0});*/
	cv::imshow("Warped", warped);

	std::vector<VertexCoord> vertexCoords = GenerateVertexCoords();
	for (auto& c : vertexCoords) {
		auto m = mapper(c);

		cv::Mat destinationROI = copyImageAreaContainingVertex(warped, m);
	}

    //cv:imshow("Warped after cutting of verticies", warped);


	//---- testing removing board to extract elements lying on it ----//

	cv::Mat emptyBoard = cv::imread("resources/empty_board1.jpg");
	cv::Mat boardWithPawns = cv::imread("resources/board_with_pawns1.jpg");
	emptyBoard = scaleImage(emptyBoard, 0.2);
	boardWithPawns = scaleImage(boardWithPawns, 0.2);
	cv::imshow("Empty board", emptyBoard);
	cv::imshow("With pawns", boardWithPawns);


	cv::Mat warpedEmptyBoard;
	try {
		warpedEmptyBoard = detector.findBoard(emptyBoard).value();
	}
	catch (std::bad_optional_access& ex) {
		std::cerr << "error: board not found";
		return 1;
	}
	cv::Mat warpedBoardWithPawns;
	try {
		warpedBoardWithPawns = detector.findBoard(boardWithPawns).value();
	}
	catch (std::bad_optional_access& ex) {
		std::cerr << "error: board not found";
		return 1;
	}

	warpedEmptyBoard = rotateImage(warpedEmptyBoard, -60);
	cv::imshow("Prepared empty", warpedEmptyBoard);

	warpedBoardWithPawns = rotateImage(warpedBoardWithPawns, -120);
	cv::imshow("Prepared with pawns", warpedBoardWithPawns);

	cv::Mat mask = findAMaskForElementsOnBoard(warpedEmptyBoard, warpedBoardWithPawns);
	cv::imshow("Mask", mask);
	
	cv::waitKey();
	
}
