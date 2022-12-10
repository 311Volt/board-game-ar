#include <catan.hpp>

std::map<std::string, cv::Mat> ResourceContainer;

void ctn::InitBitmapResources(const std::map<std::string, cv::Mat> &resources)
{
	ResourceContainer = resources;
}

cv::Mat ctn::GetBitmapResource(const std::string &name)
{
	return ResourceContainer.at(name);
}
