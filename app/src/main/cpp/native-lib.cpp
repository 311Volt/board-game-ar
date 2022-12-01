#include <jni.h>
#include <string>

#include <vector>
#include "BitmapDecoder.hpp"

#include <catan.hpp>
#include <optional>

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
Java_pg_eti_arapp_catan_CatanBoardDetector_analyzeToImage(JNIEnv *env, jobject thiz, jobject bitmap) {
    cv::Mat mat = FromJavaBitmap(env, bitmap);

    cv::Mat result = NEW_MAT(tmp) {cv::resize(mat, tmp, {1280, 960});};

    CatanBoardDetector detector(SEA_COLOR_YCBCR_6500K);
    auto optWarped = detector.findBoard(result);
    if(optWarped) {
        auto warped = optWarped.value();
        auto optInfo = ctn::AnalyzeBoard(ctn::CreateBoardIR(warped));
        ctn::DrawBoardInfo(optInfo, warped);
        result = warped;
    }


    return ToJavaBitmap(env, NEW_MAT(tmp) {cv::resize(result, tmp, {640,480});});
}
extern "C"
JNIEXPORT void JNICALL
Java_pg_eti_arapp_catan_CatanBoardDetector_initializeDetectorNative(JNIEnv *env, jclass clazz, jobject dict) {
    ctn::InitBitmapResources(FromJavaBitmapDict(env, dict));
}
extern "C"
JNIEXPORT jstring JNICALL
Java_pg_eti_arapp_catan_CatanBoardDetector_analyzeNative(JNIEnv *env, jobject thiz, jobject bitmap) {
    cv::Mat mat = FromJavaBitmap(env, bitmap);

    cv::Mat result = NEW_MAT(tmp) {cv::resize(mat, tmp, {1280, 960});};

    CatanBoardDetector detector(SEA_COLOR_YCBCR_6500K);
    auto optWarped = detector.findBoard(result);
    if(optWarped) {
        auto warped = optWarped.value();
        auto optInfo = ctn::AnalyzeBoard(ctn::CreateBoardIR(warped));
        ctn::DrawBoardInfo(optInfo, warped);
        return env->NewStringUTF(ctn::SerializeBoardInfo(optInfo).c_str());
    }

    return env->NewStringUTF("");
}