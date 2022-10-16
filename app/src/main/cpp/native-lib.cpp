#include <jni.h>
#include <string>

#include <vector>

#include <opencv2/core.hpp>

extern "C" JNIEXPORT jstring JNICALL
Java_pg_eti_arapp_FirstFragment_stringFromJNI(
        JNIEnv* env,
        jobject /* this */
)
{
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}