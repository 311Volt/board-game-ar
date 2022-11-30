
#ifndef TEST2_BITMAPDECODER_HPP
#define TEST2_BITMAPDECODER_HPP

#include <opencv2/opencv.hpp>
#include <jni.h>

cv::Mat FromJavaBitmap(JNIEnv* env, jobject bmp);
jobject ToJavaBitmap(JNIEnv* env, const cv::Mat& mat);

std::map<std::string, cv::Mat> FromJavaBitmapDict(JNIEnv* env, jobject dict);
jobject ToJavaBitmapDict(JNIEnv* env, const std::map<std::string, cv::Mat>& bitmaps);

#endif //TEST2_BITMAPDECODER_HPP
