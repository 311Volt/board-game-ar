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
	emptyBoard = scaleImage(emptyBoard, 0.3);
	boardWithPawns = scaleImage(boardWithPawns, 0.3);
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

	//cv::GaussianBlur(warpedEmptyBoard, warpedEmptyBoard, cv::Size(5, 5), cv::BORDER_DEFAULT);
	//cv::GaussianBlur(warpedBoardWithPawns, warpedBoardWithPawns, cv::Size(5, 5), cv::BORDER_DEFAULT);

	cv::Mat mask = findAMaskForWhitePawns(warpedBoardWithPawns);
	cv::imshow("Mask for white", mask);

	mask = findAMaskForElementsOnBoardHSV(warpedEmptyBoard, warpedBoardWithPawns);
	cv::imshow("Mask for everything on board", mask);

	mask = findAMaskForElementsOnBoard(warpedEmptyBoard, warpedBoardWithPawns);
	cv::imshow("Mask for everything on board (BGR)", mask);

	mask = findAMaskForElementsOnBoardKernel(warpedEmptyBoard, warpedBoardWithPawns, 13);
	//mask = findAMaskForElementsOnBoardKernelHSV(warpedEmptyBoard, warpedBoardWithPawns, 13);
	cv::imshow("Mask with kernel", mask);

	cv::cvtColor(mask, mask, cv::COLOR_BGR2GRAY);
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(mask, contours, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);
	std::vector<std::vector<cv::Point>> filteredContours;
	for (auto& cnt : contours)
	{
		if (cv::contourArea(cnt) >= 100)
			filteredContours.push_back(cnt);
	}
	cv::Mat filteredMask = cv::Mat::zeros(warpedBoardWithPawns.size(), warpedBoardWithPawns.type());
	cv::drawContours(filteredMask, filteredContours, -1, cv::Scalar(255, 255, 255), -1);
	cv::imshow("Filtered mask", filteredMask);
	
	cv::waitKey();
	
}
