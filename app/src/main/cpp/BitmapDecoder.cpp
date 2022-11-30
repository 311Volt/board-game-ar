
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


std::vector<std::string> ReadStringArray(JNIEnv* env, jobject arrObj)
{
    std::vector<std::string> ret;
    auto arr = (jobjectArray)arrObj;
    auto length = env->GetArrayLength(arr);
    for(int i=0; i<length; i++) {
        auto key = (jstring)env->GetObjectArrayElement(arr, i);
        jboolean isCopy;
        const char* chars = env->GetStringUTFChars(key, &isCopy);
        ret.emplace_back(chars);
        if(isCopy == JNI_TRUE) {
            env->ReleaseStringUTFChars(key, chars);
        }
    }
    return ret;
}

jobject ToJavaBitmapDict(JNIEnv* env, const std::map<std::string, cv::Mat>& bitmaps)
{
    jclass dictClass = env->FindClass("pg/eti/arapp/detectortl/BufferBitmapDict");
    jmethodID dictCtor = env->GetMethodID(dictClass, "<init>", "()V");

    jobject resultDict = env->NewObject(dictClass, dictCtor);
    jmethodID mPutBitmap = env->GetMethodID(dictClass, "putBitmap", "(Ljava/lang/String;Lpg/eti/arapp/detectortl/BufferBitmap;)V");

    for(const auto& [key, bitmap]: bitmaps) {
        env->CallVoidMethod(resultDict, mPutBitmap, env->NewStringUTF(key.c_str()), ToJavaBitmap(env, bitmap));
    }

    return resultDict;
}


std::map<std::string, cv::Mat> FromJavaBitmapDict(JNIEnv* env, jobject dict)
{
    std::map<std::string, cv::Mat> ret;

    jclass dictClass = env->FindClass("pg/eti/arapp/detectortl/BufferBitmapDict");
    jmethodID getKeysMethod = env->GetMethodID(dictClass, "getKeySet", "()[Ljava/lang/String;");
    jmethodID getBitmapMethod = env->GetMethodID(dictClass, "getBitmap", "(Ljava/lang/String;)Lpg/eti/arapp/detectortl/BufferBitmap;");

    auto keys = ReadStringArray(env, env->CallObjectMethod(dict, getKeysMethod));

    for(const auto& key: keys) {
        jobject bmp = env->CallObjectMethod(dict, getBitmapMethod, env->NewStringUTF(key.c_str()));

        ret[key] = FromJavaBitmap(env, bmp);
    }

    return ret;
}