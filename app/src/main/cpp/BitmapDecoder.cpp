//
// Created by volt on 30/10/2022.
//

#include "BitmapDecoder.hpp"
#include <stdint.h>
#include <jni.h>

cv::Mat FromJavaBitmap(JNIEnv* env, jobject bmp)
{
    auto bmpClass = env->GetObjectClass(bmp);
    int width = env->GetIntField(bmp, env->GetFieldID(bmpClass, "width", "I"));
    int height = env->GetIntField(bmp, env->GetFieldID(bmpClass, "height", "I"));

    jobject dataFieldId = env->GetObjectField(bmp, env->GetFieldID(bmpClass, "data", "[B"));
    auto dataArr = static_cast<jbyteArray>(dataFieldId);

    jbyte* dat = env->GetByteArrayElements(dataArr, nullptr);

    cv::Mat outputRef(height, width, CV_8UC4, dat);
    cv::Mat output;
    cv::cvtColor(outputRef, output, cv::COLOR_BGRA2RGB);

    env->ReleaseByteArrayElements(dataArr, dat, 0);

    return output;
}

jobject ToJavaBitmap(JNIEnv* env, const cv::Mat& mat)
{
    cv::Mat argb8888;
    cv::cvtColor(mat, argb8888, cv::COLOR_RGB2BGRA);

    if(!argb8888.isContinuous())
        return nullptr;

    size_t matNumBytes = argb8888.total() * argb8888.elemSize();
    jbyteArray arr = env->NewByteArray(matNumBytes);
    env->SetByteArrayRegion(arr, 0, matNumBytes, reinterpret_cast<const jbyte *>(argb8888.ptr()));

    jclass bmpClass = env->FindClass("pg/eti/arapp/detectortl/BufferBitmap");
    jmethodID bitmapCtor = env->GetMethodID(bmpClass, "<init>", "(II[B)V");
    return env->NewObject(bmpClass, bitmapCtor, argb8888.cols, argb8888.rows, arr);
}