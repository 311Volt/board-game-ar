#include <opencv2/opencv.hpp>


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

	for (int y = 0; y < source.rows; y++) {
		uint8_t* outRow = out.ptr<uint8_t>(y);
		cv::Vec3b* inRow = source.ptr<cv::Vec3b>(y);
		for (int x = 0; x < source.cols; x++) {
			cv::Vec3f diff = cv::Vec3f(inRow[x]) - vec;
			outRow[x] = cv::norm(diff);
		}
	}

	return out;
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