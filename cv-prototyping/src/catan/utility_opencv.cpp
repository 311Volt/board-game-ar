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

cv::Mat cvutil::ConvertToCrCb(cv::Mat image)
{
	cv::Mat ycrcb;
	cv::cvtColor(image, ycrcb, cv::COLOR_BGR2YCrCb);
	cv::Mat ch[3];
	cv::split(ycrcb, ch);
	ch[0] *= 0.0f;
	cv::merge(ch, 3, ycrcb);
	return ycrcb;
}

cv::Vec3b cvutil::YCrCbOf(cv::Vec3b bgr)
{
	cv::Mat tmp = cv::Mat::zeros(1,1,CV_8UC3);
	tmp.at<cv::Vec3b>(0,0) = bgr;
	cv::cvtColor(tmp, tmp, cv::COLOR_BGR2YCrCb);
	cv::Vec3b ret = tmp.at<cv::Vec3b>(0,0);
	return ret;
}

cv::Vec3b cvutil::CrCbOf(cv::Vec3b bgr)
{
	cv::Vec3b ret = YCrCbOf(bgr);
	ret[0] = 0;
	return ret;
}


cv::Mat cvutil::Convert(cv::Mat image, int code)
{
	return NEW_MAT(tmp) {
		cv::cvtColor(image, tmp, code);
	};
}

cv::Mat cvutil::ToFloat(const cv::Mat &input)
{
	return NEW_MAT(tmp) {input.convertTo(tmp, CV_32F, 1.0f / 255.0f);};
}

cvutil::MeanStdDev cvutil::MeanStdDevF32(cv::Mat a)
{
	if(a.type() != CV_32FC1) {
		throw std::runtime_error("MeanStdDevF32 called with invalid Mat type");
	}

	cv::Scalar m, d;
	cv::meanStdDev(a, m, d);
	return {.mean = m[0], .stddev = d[0]};
}

std::array<cvutil::MeanStdDev, 3> cvutil::MeanStdDevBGR(cv::Mat bgr)
{
	std::array<cvutil::MeanStdDev, 3> ret;
	auto split = SplitBGR(bgr);
	for(int i=0; i<3; i++) {
		ret[i] = MeanStdDevF32(split[i]);
	}
	return ret;
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
