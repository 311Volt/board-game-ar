#ifndef INCLUDE_CATAN_UTILITY_OPENCV
#define INCLUDE_CATAN_UTILITY_OPENCV

#include <opencv2/opencv.hpp>
#include <concepts>
#include <optional>

namespace cvutil {
	struct dummy {};

	struct MeanStdDev {
		double mean, stddev;
	};

	template<std::invocable<cv::Mat&> FuncT>
	cv::Mat operator+(dummy, FuncT func)
	{
		cv::Mat output;
		func(output);
		return output;
	}

	namespace cfg {
		struct WindowConfig {
			bool waitKeyOnExit = false;
		};

		struct ShowOptions {
			bool waitKey = false;
			std::optional<cv::Vec2i> scale = std::nullopt;
		};

		struct DrawPointsOptions {
			cv::Scalar color = {0, 255, 255};
			float radius = 10.0f;
		};
	}

	struct Window {
		cfg::WindowConfig config;
		std::string name;

		Window(const std::string& name, cfg::WindowConfig cfg = {});
		~Window();

		void show(cv::Mat img, cfg::ShowOptions options = {});
	};

	void drawPoints(const std::vector<cv::Point2d>& points, cv::Mat outImg, cfg::DrawPointsOptions options = {});
	cv::Mat convertToCrCb(cv::Mat image);
	cv::Mat Convert(cv::Mat image, int code);
	cv::Mat ToFloat(const cv::Mat& input);
	cv::Mat ToByte(const cv::Mat& input);
	MeanStdDev MeanStdDevF32(cv::Mat a);
	std::array<cvutil::MeanStdDev, 3> MeanStdDevBGR(cv::Mat bgr);


	std::array<cv::Mat, 3> SplitBGR(const cv::Mat& input);
	cv::Mat MergeBGR(const std::array<cv::Mat, 3>& bgr);
}

#define NEW_MAT(tmpName__) ::cvutil::dummy() + [&](cv::Mat& tmpName__)


#endif /* INCLUDE_CATAN_UTILITY_OPENCV */
