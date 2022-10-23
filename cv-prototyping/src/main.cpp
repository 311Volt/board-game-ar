#include <iostream>

#include <opencv2/opencv.hpp>

#include <catan/board_coords.hpp>
#include <catan/board_detection.hpp>
#include <catan/image_correction.hpp>
#include <catan/utility_opencv.hpp>

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

	/*for(auto& c: GenerateFieldCoords(3)) {
		cv::putText(warped, fmt::format("({},{},{})", c.x, c.y, c.z), mapper(c), cv::FONT_HERSHEY_PLAIN, 1, {255,255,255});
	}*/

	//drawPoints(mapper(GenerateFieldCoords(2)), warped, {255,255,180});

	std::vector<VertexCoord> vertexCoords = GenerateVertexCoords();
	for (auto& c : vertexCoords) {
		//cv::putText(warped, fmt::format("({},{},{})", c.origin.x, c.origin.y, c.origin.z), mapper(c), cv::FONT_HERSHEY_PLAIN, 1, { 255,255,255 });
		auto m = mapper(c);
		cv::Point* p = new cv::Point(0, 0);
		//auto col = warped.colRange(m.x - 10, m.x + 10).rowRange(m.y - 10, m.y + 10);

		int rectSize = 48;

		cv::Rect roi(cv::Point(m.x - rectSize/2, m.y - rectSize/2), cv::Size(rectSize, rectSize));
		cv::Mat destinationROI = warped(roi);


		cv::Mat hsvFull;
		cv::cvtColor(destinationROI, hsvFull, cv::COLOR_BGR2HSV_FULL);
		std::vector<uchar> colorsCount(256, 0);
		cv::Scalar pixel;
		uchar color;
		int maxColor = 0, minColor = 255;
		for (int r = 0; r < hsvFull.rows; r++)
		{
			for (int c = 0; c < hsvFull.cols; c++)
			{
				pixel = hsvFull.at<cv::Vec3b>(cv::Point(c, r));
				color = pixel(0);
				colorsCount[color] += 1;
				if (colorsCount[color] < colorsCount[minColor]) minColor = color;
				if (colorsCount[color] > colorsCount[maxColor]) maxColor = color;
			}
		}
		int colorRange = maxColor - minColor;
		int minCount, maxCount;
		minCount = colorsCount[minColor];
		maxCount = colorsCount[maxColor];

		cv::Mat smallImage(rectSize, rectSize, CV_8UC3, cv::Scalar(255, 255, 255));
		
		cv::imshow("Dest", destinationROI);
		//cv::waitKey();

		if(maxColor - minColor < 100)
			(smallImage).copyTo(destinationROI);
		

		//auto rect = n;

		//(*rect).copyTo(col);
		int i = 0;
	}

	//drawPoints(mapper(GenerateVertexCoords()), warped, {255,0,0});
	
	/*showScaled("Source", src);
	showScaled("CrCb", crcb);
	showScaled("sqDiff(CrCb, sea color)", sq);
	showScaled("Threshold", thres);*/

	cv::imshow("Warped", warped);
	cv::waitKey();
}
