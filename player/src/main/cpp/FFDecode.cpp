#include "FFDecode.h"
#include <AndroidLogger.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavcodec/jni.h>
}

void FFDecode::InitHardDecode(void *vm) {
    av_jni_set_java_vm(vm, 0);
}

bool FFDecode::Open(PlayParams params, bool isHardDecode) {
    Close();
    if (!params.param) return false;
    AVCodecParameters *p = params.param;

    // 1. 查找解码器
    const AVCodec *codec = avcodec_find_decoder(p->codec_id);
    if (isHardDecode) {
        if (p->codec_id == AV_CODEC_ID_H264)
            codec = avcodec_find_decoder_by_name("h264_mediacodec");
        else if (p->codec_id == AV_CODEC_ID_MPEG4)
            codec = avcodec_find_decoder_by_name("mpeg4_mediacodec");
    }

    if (!codec) {
        LOGE("could not find decoder with id %d.", p->codec_id);
        return false;
    }

    mux.lock();
    // 2. 创建解码器上下文
    codecCtx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codecCtx, p);
    // 开启多线程解码
    codecCtx->thread_count = 8;

    // 3. 打开解码器
    int re = avcodec_open2(codecCtx, 0, 0);
    if (re != 0) {
        mux.unlock();
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf) - 1);
        LOGE("open codec %d failed: %s.", codec->id, buf);
        return false;
    }

    if (codecCtx->codec_type == AVMEDIA_TYPE_AUDIO) {
        this->isAudio = true;
        LOGE("find audio decoder success: %d.", codec->id);
    } else {
        this->isAudio = false;
        LOGE("find video decoder success: %d. isHardDecode: %d", codec->id, isHardDecode);
    }

    mux.unlock();

    LOGI("Open Codec %d Success.", codecCtx->codec_id);

    return true;
}

void FFDecode::Close() {
    IDecode::Clear();
    mux.lock();
    pts = 0;
    if (frame) {
        av_frame_free(&frame);
    }

    if (codecCtx) {
        avcodec_close(codecCtx);
        avcodec_free_context(&codecCtx);
    }
    mux.unlock();
}

void FFDecode::Clear() {
    IDecode::Clear();
    mux.lock();
    if (codecCtx) {
        avcodec_flush_buffers(codecCtx);
    }
    mux.unlock();
}

bool FFDecode::SendPacket(PlayData playData) {
    if (playData.size <= 0 || !playData.data) return false;
    mux.lock();
    if (!codecCtx) {
        mux.unlock();
        return false;
    }
    int re = avcodec_send_packet(codecCtx, (AVPacket *) playData.data);
    if (re != 0 && !isAudio) {
        char str[1024] = {0};
        av_strerror(re, str, sizeof(str) -1);
        LOGE("avcodec_send_packet failed!!!, %s", str);
    }
    mux.unlock();

    return re == 0;
}

PlayData FFDecode::RecvFrame() {
    mux.lock();
    if (!codecCtx) {
        mux.unlock();
        return PlayData();
    }

    if (!frame) {
        frame = av_frame_alloc();
    }

    int re = avcodec_receive_frame(codecCtx, frame);
    if (re != 0) {
        mux.unlock();
        return PlayData();
    }

    if (!isAudio) {
        // LOGI("received video frame, linesize = %d, width = %d", frame->linesize[0], frame->width);
    }

    PlayData playData;
    playData.data = reinterpret_cast<unsigned char *>(frame);
    if (codecCtx->codec_type == AVMEDIA_TYPE_VIDEO) {
        playData.size =
                (frame->linesize[0] + frame->linesize[1] + frame->linesize[2]) * frame->height;
        playData.width = frame->linesize[0];
        playData.height = frame->height;
    } else {
        playData.size =
                av_get_bytes_per_sample((AVSampleFormat) frame->format) * frame->nb_samples *
                frame->channels;
    }
    playData.format = frame->format;
    memcpy(playData.datas, frame->data, sizeof(playData.datas));
    pts = playData.pts = frame->pts;
    mux.unlock();
    return playData;
}


