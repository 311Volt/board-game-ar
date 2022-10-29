#include <catan/pawns_detection.hpp>

#include <catan/utility_opencv.hpp>
#include <catan/board_detection.hpp>
//#include <catan/image_correction.hpp>


cv::Mat copyImageAreaContainingVertex(cv::Mat& image, cv::Point2d mappedVtx)
{
	int rectSize = 60;

	cv::Rect roi(cv::Point(mappedVtx.x - rectSize / 2, mappedVtx.y - rectSize / 2), cv::Size(rectSize, rectSize));
	cv::Mat destinationROI;
	image(roi).copyTo(destinationROI);

	//making circle mask instead of square
	/*cv::Mat mask = cv::Mat::zeros(destinationROI.size(), destinationROI.type());
	cv::Point2i center = cv::Point2i(mask.size().width / 2, mask.size().height / 2);
	cv::circle(mask, center, rectSize / 2, cv::Scalar(255, 255, 255), -1);
	cv::bitwise_and(destinationROI, mask, destinationROI);*/

	//counting colors
	/*destinationROI = image(roi);
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

	//cv::imshow("Dest", destinationROI);
	//cv::waitKey();

	if (maxColor - minColor < 100)
		(smallImage).copyTo(destinationROI);*/

	return destinationROI;
}


/*bool checkIfEdgeIsStraight(cv::Mat vtxFragment)
{
	cv::Mat horizontalEdges;
	cv::Sobel(vtxFragment, horizontalEdges, CV_64F, 1, 0, 3);
	cv::imshow("Horizontal", horizontalEdges);
	return true;
}*/


//bool checkIfThereIsAPawn(cv::Mat vtxImage, int position)
//{
//	if (position == 0) // One Line Up
//	{
//		auto rotatedImage = rotateImage(vtxImage, -30);
//		cv::imshow("Rotated Vertex", rotatedImage);
//		cv::waitKey();
//		cv::Mat edgesOfCell;
//		cv::Canny(rotatedImage, edgesOfCell, 125, 175);
//		//cv::inRange(rotatedImage, cv::Scalar(0, 0, 0), cv::Scalar(100, 100, 100), edgesOfCell);
//		cv::imshow("Edges", edgesOfCell);
//		/*cv::Mat grayscale;
//		cv::cvtColor(rotatedImage, grayscale, cv::COLOR_BGR2GRAY);
//		cv::imshow("Gray", grayscale);
//		cv::Rect roi = cv::Rect(cv::Point2i(0, 0), cv::Size(grayscale.size().width / 2, grayscale.size().height));
//		cv::Mat halfOfVertexImage;
//		grayscale(roi).copyTo(halfOfVertexImage);
//		cv::imshow("Half", halfOfVertexImage);
//		cv::Mat blurred;
//		cv::GaussianBlur(halfOfVertexImage, blurred, cv::Size(3, 3), cv::BORDER_DEFAULT);
//		cv::imshow("Blur", blurred);
//		checkIfEdgeIsStraight(blurred);*/
//
//	}
//	else if (position == 1) // Two Lines Up
//	{
//
//	}
//	return true;
//}


// for now, its fixed for 3-channels images, for example BGR images
cv::Mat findAMaskForElementsOnBoard(cv::Mat emptyBoard, cv::Mat boardWithPawns)
{
	cv::Mat mask = cv::Mat::zeros(boardWithPawns.size(), boardWithPawns.type());

	//std::cout << mask.channels();
	for (int r = 0; r < emptyBoard.size().height; r++)
	{
		for (int c = 0; c < emptyBoard.size().width; c++)
		{
			cv::Vec3i pixel1 = emptyBoard.at<cv::Vec3b>(r, c);
			cv::Vec3i pixel2 = boardWithPawns.at<cv::Vec3b>(r, c);
			cv::Vec3i colorsDiff = pixel2 - pixel1;
			//colorsDiff = cv::Vec3b(std::abs(colorsDiff.val[0]), std::abs(colorsDiff.val[1]), std::abs(colorsDiff.val[2]));
			//////cv::absdiff(pixel1, pixel2, colorsDiff);
			float colorsDistance = cv::norm(colorsDiff/3);
			if (colorsDistance >= 20)
				mask.at<cv::Vec3b>(r, c) = cv::Vec3b(255, 255, 255);

		}
	}
	//cv::subtract(boardWithPawns, emptyBoard, mask);
	return mask;
}

cv::Mat convertToHS(cv::Mat image)
{
	cv::Mat hsv;
	cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV_FULL);
	std::vector<cv::Mat> channels;
	cv::split(hsv, channels);
	channels[2] = cv::Mat::zeros(hsv.size(), CV_8UC1);

	//channels[2] = cv::Mat::zeros(hsv.size(), CV_8UC1);
	//hsvEmpty[2] = cv::Mat::zeros(hsvEmpty.size(), CV_8UC1);
	cv::merge(channels, hsv);
	return hsv;
}

cv::Mat convertToSV(cv::Mat image)
{
	cv::Mat hsv;
	cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV_FULL);
	std::vector<cv::Mat> channels;
	cv::split(hsv, channels);
	channels[0] = cv::Mat::zeros(hsv.size(), CV_8UC1);
	cv::merge(channels, hsv);
	return hsv;
}

cv::Mat findAMaskForElementsOnBoardHSV(cv::Mat emptyBoard, cv::Mat boardWithPawns)
{
	cv::Mat hsvEmpty = convertToHS(emptyBoard);
	cv::imshow("Hsv empty", hsvEmpty);
	cv::Mat hsvWithPawns = convertToHS(boardWithPawns);
	cv::imshow("Hsv with pawns", hsvWithPawns);
	cv::Mat mask = cv::Mat::zeros(boardWithPawns.size(), boardWithPawns.type());
	for (int r = 0; r < hsvEmpty.size().height; r++)
	{
		for (int c = 0; c < hsvEmpty.size().width; c++)
		{
			cv::Vec3i pixel1 = hsvEmpty.at<cv::Vec3b>(r, c);
			cv::Vec3i pixel2 = hsvWithPawns.at<cv::Vec3b>(r, c);
			cv::Vec3i colorsDiff = pixel2 - pixel1;
			float colorsDistance = cv::norm(colorsDiff / 3);
			if (colorsDistance >= 20)
				mask.at<cv::Vec3b>(r, c) = cv::Vec3b(255, 255, 255);
		}
	}
	//cv::subtract(boardWithPawns, emptyBoard, mask);
	return mask;
}

cv::Mat findAMaskForWhitePawns(cv::Mat boardWithPawns)//(cv::Mat emptyBoard, cv::Mat boardWithPawns)
{
	//cv::Mat hsvEmpty = convertToHS(emptyBoard);
	//cv::imshow("Hsv empty", hsvEmpty);
	cv::Mat hsvWithPawns = convertToSV(boardWithPawns);
	cv::imshow("Hsv with pawns", hsvWithPawns);
	cv::Mat mask = cv::Mat::zeros(boardWithPawns.size(), CV_8UC1);
	cv::Mat Smask = cv::Mat::zeros(boardWithPawns.size(), CV_8UC1);
	cv::Mat Vmask = cv::Mat::zeros(boardWithPawns.size(), CV_8UC1);

	//std::vector<cv::Mat> channels1;
	std::vector<cv::Mat> channels2;
	//cv::split(hsvEmpty, channels1);
	cv::split(hsvWithPawns, channels2);
	cv::threshold(channels2[1], Smask, 30, 255, cv::THRESH_BINARY_INV);
	cv::threshold(channels2[2], Vmask, 150, 255, cv::THRESH_BINARY);
	cv::bitwise_and(Smask, Vmask, mask);
	cv::imshow("Smask", Smask);
	cv::imshow("Vmask", Vmask);

	return mask;

}

cv::Mat findAMaskForElementsOnBoardKernel(cv::Mat emptyBoard, cv::Mat boardWithPawns, int kernel)
{
	//cv::Mat hsvEmpty = convertToHS(emptyBoard);
	//cv::imshow("Hsv empty", hsvEmpty);
	//cv::Mat hsvWithPawns = convertToHS(boardWithPawns);
	//cv::imshow("Hsv with pawns", hsvWithPawns);
	cv::Mat mask = cv::Mat::zeros(boardWithPawns.size(), boardWithPawns.type());
	bool similarColor = false;
	for (int r = 0; r < emptyBoard.size().height; r++)
	{
		for (int c = 0; c < emptyBoard.size().width; c++)
		{
			similarColor = false;
			for (int x = 0; x < kernel; x++)
			{
				for (int y = 0; y < kernel; y++)
				{
					if (r + x - (kernel / 2) >= 0 && r + x - (kernel / 2) < emptyBoard.size().height && c + y - (kernel / 2) >= 0 && c + y - (kernel / 2) < emptyBoard.size().width)
					{
						cv::Vec3i pixel1 = emptyBoard.at<cv::Vec3b>(r + x - (kernel / 2), c + y - (kernel / 2));
						cv::Vec3i pixel2 = boardWithPawns.at<cv::Vec3b>(r, c);
						cv::Vec3i colorsDiff = pixel2 - pixel1;
						float colorsDistance = cv::norm(colorsDiff / 3);
						if (colorsDistance <= 15)
							similarColor = true;
					}
				}
			}
			if (!similarColor)
				mask.at<cv::Vec3b>(r, c) = cv::Vec3b(255, 255, 255);
		}
	}
	return mask;
}

cv::Mat findAMaskForElementsOnBoardKernelHSV(cv::Mat emptyBoard, cv::Mat boardWithPawns, int kernel)
{
	cv::Mat hsvEmpty = convertToHS(emptyBoard);
	cv::imshow("Hsv empty", hsvEmpty);
	cv::Mat hsvWithPawns = convertToHS(boardWithPawns);
	cv::imshow("Hsv with pawns", hsvWithPawns);
	cv::Mat mask = cv::Mat::zeros(boardWithPawns.size(), boardWithPawns.type());
	bool similarColor = false;
	for (int r = 0; r < hsvEmpty.size().height; r++)
	{
		for (int c = 0; c < hsvEmpty.size().width; c++)
		{
			similarColor = false;
			for (int x = 0; x < kernel; x++)
			{
				for (int y = 0; y < kernel; y++)
				{
					if (r + x - (kernel / 2) >= 0 && r + x - (kernel / 2) < emptyBoard.size().height && c + y - (kernel / 2) >= 0 && c + y - (kernel / 2) < emptyBoard.size().width)
					{
						cv::Vec3i pixel1 = hsvEmpty.at<cv::Vec3b>(r + x - (kernel / 2), c + y - (kernel / 2));
						cv::Vec3i pixel2 = hsvWithPawns.at<cv::Vec3b>(r, c);
						cv::Vec3i colorsDiff = pixel2 - pixel1;
						float colorsDistance = cv::norm(colorsDiff / 3);
						if (colorsDistance <= 10)
							similarColor = true;
					}
				}
			}
			if (!similarColor)
				mask.at<cv::Vec3b>(r, c) = cv::Vec3b(255, 255, 255);
		}
	}
	return mask;
}