#ifndef INCLUDE_CATAN_RESOURCE_PROVIDER
#define INCLUDE_CATAN_RESOURCE_PROVIDER

#include <opencv2/opencv.hpp>
#include <map>

namespace ctn {

	void InitBitmapResources(const std::map<std::string, cv::Mat>& resources);
	cv::Mat GetBitmapResource(const std::string& name);
}

#endif /* INCLUDE_CATAN_RESOURCE_PROVIDER */
