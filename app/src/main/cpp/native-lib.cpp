#include <jni.h>
#include <string>

#include <vector>

extern "C" JNIEXPORT jstring JNICALL
Java_butowska_anna_test2_FirstFragment_stringFromJNI(
        JNIEnv* env,
        jobject /* this */
)
{
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}