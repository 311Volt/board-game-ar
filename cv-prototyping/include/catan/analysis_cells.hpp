#ifndef INCLUDE_CATAN_ANALYSIS_CELLS
#define INCLUDE_CATAN_ANALYSIS_CELLS

#include <opencv2/opencv.hpp>

namespace ctn {
	cv::Mat GenerateCellMask(double mu, double sigma);
};

#endif /* INCLUDE_CATAN_ANALYSIS_CELLS */
