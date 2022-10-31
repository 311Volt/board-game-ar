#include <catan/utility_opencv.hpp>


void cvutil::drawPoints(const std::vector<cv::Point2d>& points, cv::Mat outImg, cvutil::cfg::DrawPointsOptions options)
{
	for (const auto& p : points) {
		cv::circle(outImg, p, options.radius, options.color, -1);
	}
}

cvutil::Window::Window(const std::string& name, cvutil::cfg::WindowConfig cfg)
	: name(name), config(cfg)
{

}

cvutil::Window::~Window()
{
	if(config.waitKeyOnExit) {
		cv::waitKey();
	}
}

void cvutil::Window::show(cv::Mat img, cvutil::cfg::ShowOptions options)
{
	if(options.scale.has_value()) {
		cv::resizeWindow(name, (*options.scale)[0], (*options.scale)[1]);
	}
	cv::imshow(name, img);
	if(options.waitKey) {
		cv::waitKey();
	}
}

cv::Mat cvutil::convertToCrCb(cv::Mat image)
{
	cv::Mat ycrcb;
	cv::cvtColor(image, ycrcb, cv::COLOR_BGR2YCrCb);
	cv::Mat ch[3];
	cv::split(ycrcb, ch);
	ch[0] *= 0.0f;
	cv::merge(ch, 3, ycrcb);
	return ycrcb;
}

cv::Mat cvutil::ToFloat(const cv::Mat &input)
{
	return NEW_MAT(tmp) {input.convertTo(tmp, CV_32F, 1.0f / 255.0f);};
}


cv::Mat cvutil::ToByte(const cv::Mat &input)
{
	return NEW_MAT(tmp) {input.convertTo(tmp, CV_8U, 255);};
}


std::array<cv::Mat, 3> cvutil::SplitBGR(const cv::Mat& input)
{
	std::array<cv::Mat, 3> result;
	cv::split(input, result.data());
	return result;
}
cv::Mat cvutil::MergeBGR(const std::array<cv::Mat, 3>& bgr)
{
	cv::Mat output;
	cv::merge(bgr.data(), 3, output);
	return output;
}
