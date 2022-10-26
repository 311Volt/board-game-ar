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
	ch[0] = cv::Mat::zeros(ch[0].rows, ch[0].cols, CV_8UC1);
	cv::merge(ch, 3, ycrcb);
	return ycrcb;
}
