#include <jni.h>
#include <string>
#include <unistd.h>
#include <AndroidLogger.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libavcodec/jni.h>

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

}
