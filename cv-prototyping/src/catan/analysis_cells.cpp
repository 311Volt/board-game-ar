#include <catan/analysis_cells.hpp>

float rating1(float x)
{
    return 0.5f - std::atan(4.0f*x - 5) / 3.141591f;
}

cv::Mat ctn::GenerateCellMask(double mu, double sigma)
{
	cv::Mat mask = cv::Mat::zeros({130, 150}, CV_32FC1);
	cv::Vec2f center {mask.cols*0.5f, mask.rows*0.5f};

	mu *= mu;

	for(int y=0; y<mask.rows; y++) {
		float* row = mask.ptr<float>(y);
		for(int x=0; x<mask.cols; x++) {
			float dx = x-center[0];
			float dy = y-center[1];

			float sqDist = dx*dx + dy*dy;
			float dfd = std::abs<float>(mu - sqDist);

			row[x] = rating1(dfd / sigma);
			row[x] *= row[x];
		}
	}

	return mask;
}
