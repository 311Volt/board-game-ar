#include <jni.h>
#include <string>

#include <vector>
#include "BitmapDecoder.hpp"

#include <catan.hpp>
#include "cards_recognition.hpp"
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

void yuv420toNv21(int image_width, int image_height, const int8_t* y_buffer,
                  const int8_t* u_buffer, const int8_t* v_buffer, int y_pixel_stride,
                  int uv_pixel_stride, int y_row_stride, int uv_row_stride,
                  int8_t *nv21) {
    for(int y = 0; y < image_height; ++y) {
        int destOffset = image_width * y;
        int yOffset = y * y_row_stride;
        memcpy(nv21 + destOffset, y_buffer + yOffset, image_width);
    }

    int idUV = image_width * image_height;
    int uv_width = image_width / 2;
    int uv_height = image_height / 2;
    for(int y = 0; y < uv_height; ++y) {
        int uvOffset = y * uv_row_stride;
        for (int x = 0; x < uv_width; ++x) {
            int bufferIndex = uvOffset + (x * uv_pixel_stride);
            nv21[idUV++] = v_buffer[bufferIndex];
            nv21[idUV++] = u_buffer[bufferIndex];
        }
    }
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_pg_eti_arapp_utils_YuvConverter_yuv420_1888toNv21(JNIEnv *env, jclass clazz,
                                                    jint image_width, jint image_height, jobject y_byte_buffer,
                                                    jobject u_byte_buffer, jobject v_byte_buffer, jint y_pixel_stride,
                                                    jint uv_pixel_stride, jint y_row_stride, jint uv_row_stride,
                                                    jbyteArray nv21_array) {
    auto y_buffer = static_cast<jbyte*>(env->GetDirectBufferAddress(y_byte_buffer));
    auto u_buffer = static_cast<jbyte*>(env->GetDirectBufferAddress(u_byte_buffer));
    auto v_buffer = static_cast<jbyte*>(env->GetDirectBufferAddress(v_byte_buffer));

    jbyte* nv21 = env->GetByteArrayElements(nv21_array, nullptr);
    if (nv21 == nullptr || y_buffer == nullptr || u_buffer == nullptr
        || v_buffer == nullptr) {
        // Log this.
        return false;
    }

    yuv420toNv21(image_width, image_height, y_buffer, u_buffer, v_buffer,
                 y_pixel_stride, uv_pixel_stride, y_row_stride, uv_row_stride,
                 nv21);

    env->ReleaseByteArrayElements(nv21_array, nv21, 0);
    return true;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_pg_eti_arapp_catan_CatanCardsDetector_getCardsNative(JNIEnv *env, jobject thiz,
                                                          jobject bitmap) {
    // TODO: implement getCardsNative()
    cv::Mat mat = FromJavaBitmap(env, bitmap);

    /*if(mat.empty())
        return nullptr;*/

    std::vector<cv::Mat> cards = getCardsFromImage(mat, true);

    jclass java_util_ArrayList = env->FindClass("java/util/ArrayList");
    jmethodID java_util_ArrayList_ = env->GetMethodID(java_util_ArrayList, "<init>", "(I)V");
    jmethodID java_util_ArrayList_add  = env->GetMethodID(java_util_ArrayList, "add", "(Ljava/lang/Object;)Z");
    jobject result = env->NewObject(java_util_ArrayList, java_util_ArrayList_, (jint)cards.size());

    if(cards.empty())
        return nullptr;

    for(const auto& card : cards)
    {
        env->CallBooleanMethod(result, java_util_ArrayList_add, ToJavaBitmap(env, card));
    }

    return result;
}