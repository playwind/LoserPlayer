#include <jni.h>
#include <string>
#include <unistd.h>
#include <AndroidLogger.h>
#include <android/native_window_jni.h>
#include "IPlayerProxy.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libavcodec/jni.h>

JNIEXPORT
jint JNI_OnLoad(JavaVM *vm, void *res) {
    // 初始化ffmpeg硬解码，创建player
    IPlayerProxy::Get()->Init(vm);
    return JNI_VERSION_1_4;
}

JNIEXPORT jstring JNICALL
Java_com_playwind_loserplayer_LoserPlayer_getPlayerInfo(JNIEnv *env, jobject /* this */) {
    LOGE("%s", "Java_com_playwind_loserplayer_LoserPlayer_getPlayerInfo");
    char info[40000];
    void *iter = NULL;
    const AVCodec *codec;
    bool isDecoder = false;

    sprintf(info, "%s", "@section Generic codec AVOptions\n");

    while ((codec = av_codec_iterate(&iter))) {
        if (codec->decode != NULL) {
            isDecoder = true;
        } else {
            isDecoder = false;
        }

        if (isDecoder) {
            sprintf(info, "%s[Decoder: %s]\n", info, codec->name);
        } else {
            sprintf(info, "%s[Encoder: %s]\n", info, codec->name);
        }
    }
    std::string str = "aaaa";
    sprintf(info, "%s@section Generic codec AVOptions\n%s", info, str.c_str());
    return env->NewStringUTF(info);
}


JNIEXPORT void JNICALL
Java_com_playwind_loserplayer_LoserPlayer_initView(JNIEnv *env, jobject thiz, jobject surface) {
    ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
    IPlayerProxy::Get()->InitView(window);
}

JNIEXPORT void JNICALL
Java_com_playwind_loserplayer_LoserPlayer_open(JNIEnv *env, jobject thiz, jstring _url) {
    const char *url = env->GetStringUTFChars(_url, 0);
    auto player = IPlayerProxy::Get();
    player->Open(url);
    player->Start();

    env->ReleaseStringUTFChars(_url, url);
}

JNIEXPORT void JNICALL
Java_com_playwind_loserplayer_LoserPlayer_pause(JNIEnv *env, jobject thiz) {
    IPlayerProxy::Get()->Pause(true);
}


JNIEXPORT void JNICALL
Java_com_playwind_loserplayer_LoserPlayer_resume(JNIEnv *env, jobject thiz) {
    IPlayerProxy::Get()->Pause(false);
}
}