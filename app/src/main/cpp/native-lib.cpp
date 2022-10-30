#include <jni.h>
#include <string>

#include <vector>
#include "BitmapDecoder.hpp"

extern "C" JNIEXPORT jstring JNICALL
Java_pg_eti_arapp_FirstFragment_stringFromJNI(
        JNIEnv* env,
        jobject /* this */
)
{
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT jobject JNICALL
Java_pg_eti_arapp_catan_CatanBoardDetector_detectBoard(JNIEnv *env, jobject thiz, jobject bitmap) {
    cv::Mat mat = FromJavaBitmap(env, bitmap);
/*
    double ir=0, ig=0, ib=0;

    for(int y=0; y<mat.rows; y++) {
        cv::Vec3b* row = mat.ptr<cv::Vec3b>(y);
        for(int x=0; x<mat.cols; x++) {
            ir += row[x][2];
            ig += row[x][1];
            ib += row[x][0];
        }
    }

    ir /= mat.cols*mat.rows;
    ig /= mat.cols*mat.rows;
    ib /= mat.cols*mat.rows;

    char buf[4096];
    sprintf(buf, "img %dx%d, intensities: r=%.3f,g=%.3f,b=%.3f", mat.cols, mat.rows, ir, ig, ib);
    */
    cv::Mat mat1;
    cv::resize(mat, mat1, {320, 240});
    cv::cvtColor(mat1, mat1, cv::COLOR_BGR2HSV);
    cv::putText(mat1, "hello from", {10, 30}, cv::FONT_HERSHEY_COMPLEX, 1, {0,255,0}, 2);
    cv::putText(mat1, "native OpenCV", {10, 55}, cv::FONT_HERSHEY_COMPLEX, 1, {0,255,0}, 2);


    return ToJavaBitmap(env, mat1);
}