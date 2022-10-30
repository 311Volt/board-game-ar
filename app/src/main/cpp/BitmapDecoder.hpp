
#ifndef TEST2_BITMAPDECODER_HPP
#define TEST2_BITMAPDECODER_HPP

#include <opencv2/opencv.hpp>
#include <jni.h>

cv::Mat FromJavaBitmap(JNIEnv* env, jobject bmp);
jobject ToJavaBitmap(JNIEnv* env, const cv::Mat& mat);

#endif //TEST2_BITMAPDECODER_HPP
