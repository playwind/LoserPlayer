#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnusedMacroInspection"
#ifndef ANDROID_LOGGER_H
#define ANDROID_LOGGER_H

#include <android/log.h>

#define JNI_TAG "LoserPlayer"

#define LOGE(format, ...) __android_log_print(ANDROID_LOG_ERROR, JNI_TAG, format, ##__VA_ARGS__)
#define LOGW(format, ...) __android_log_print(ANDROID_LOG_WARN, JNI_TAG, format, ##__VA_ARGS__)
#define LOGD(format, ...) __android_log_print(ANDROID_LOG_DEBUG, JNI_TAG, format, ##__VA_ARGS__)
#define LOGI(format, ...) __android_log_print(ANDROID_LOG_INFO, JNI_TAG, format, ##__VA_ARGS__)
#define LOGV(format, ...) __android_log_print(ANDROID_LOG_VERBOSE, JNI_TAG, format, ##__VA_ARGS__)

#endif //ANDROID_LOGGER_H

#pragma clang diagnostic pop