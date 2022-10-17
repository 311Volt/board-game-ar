#include <iostream>
#include <opencv2/opencv.hpp>

//sea: 38 110 160 (CrCb: 89, 163)

inline const cv::Vec3f SEA_COLOR_YCBCR_6500K = {0, 89, 163};
inline const cv::Vec3f SEA_COLOR_YCBCR_3400K = {0, 126, 132};


cv::Mat convertToCrCb(cv::Mat image)
{
	cv::Mat ycrcb;
	cv::cvtColor(image, ycrcb, cv::COLOR_BGR2YCrCb);
	cv::Mat ch[3];
	cv::split(ycrcb, ch);
	ch[0] = cv::Mat::zeros(ch[0].rows, ch[0].cols, CV_8UC1);
	cv::merge(ch, 3, ycrcb);
	return ycrcb;
}

cv::Mat squareDist(cv::Mat source, cv::Vec3f vec)
{
	cv::Mat out = cv::Mat::zeros(source.rows, source.cols, CV_8UC1);
	
	for(int y=0; y<source.rows; y++) {
		uint8_t* outRow = out.ptr<uint8_t>(y);
		cv::Vec3b* inRow = source.ptr<cv::Vec3b>(y);
		for(int x=0; x<source.cols; x++) {
			cv::Vec3f diff = cv::Vec3f(inRow[x]) - vec;
			outRow[x] = cv::norm(diff);
		}
	}

	return out;
}

cv::Mat getPerspectiveCorrectionMatrix(const std::vector<cv::Point>& points)
{
	std::vector<cv::Point> dstPoints = {
		{0, 433},
		{250, 866},
		{750, 866},
		{1000, 433},
		{750, 0},
		{250, 0}
	};
	return cv::findHomography(points, dstPoints);
}

void showScaled(std::string name, cv::Mat mat)
{
	cv::namedWindow(name, cv::WINDOW_NORMAL);
	cv::imshow(name, mat);
	cv::resizeWindow(name, 640, 480);
}

std::vector<cv::Point> findBoardVertices(cv::Mat thres)
{
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Point> hex;
	cv::findContours(thres, contours, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

	auto it = std::max_element(contours.begin(), contours.end(), [](auto&& a, auto&& b) {
		return cv::contourArea(a) < cv::contourArea(b);
	});

	int index = it - contours.begin();

	auto factor = 0.03;
	cv::approxPolyDP(contours[index], hex, cv::arcLength(contours[index], true) * factor, true);
	return hex;
}

int main()
{
	auto src = cv::imread("resources/thicc.jpg");
	cv::Mat crcb = convertToCrCb(src);
	cv::Mat sq = squareDist(crcb, SEA_COLOR_YCBCR_6500K);
	//cv::Mat sq = squareDist(crcb, SEA_COLOR_YCBCR_3400K);
	
	cv::Mat thres;
	cv::threshold(sq, thres, 20, 255, cv::THRESH_BINARY_INV);

	std::vector<std::vector<cv::Point>> hex = {findBoardVertices(thres)};
	cv::drawContours(src, hex, 0, {255,0,255}, 7);
	
	cv::Mat corr = getPerspectiveCorrectionMatrix(hex[0]);
	cv::Mat final;
	cv::warpPerspective(src, final, corr, {1000, 866});

	showScaled("Source", src);
	showScaled("CrCb", crcb);
	showScaled("sqDiff(CrCb, sea color)", sq);
	showScaled("Threshold", thres);
	cv::imshow("Warped", final);

	cv::waitKey();
}
