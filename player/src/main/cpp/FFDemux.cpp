#include "FFDemux.h"
#include <AndroidLogger.h>

extern "C" {
#include <libavformat/avformat.h>
}

// 分数转为浮点数
static double r2d(AVRational r) {
    return r.num == 0 || r.den == 0 ? 0 : (double) r.num / (double) r.den;
}

FFDemux::FFDemux() {
    static bool isFirst = true;
    if (!isFirst) return;

    isFirst = false;

    // 注册所有封装器，编解码器 (废弃)
    // av_register_all();

    // 初始化网络
    avformat_network_init();
}

bool FFDemux::open(const char *url) {
    LOGI("Demux open file %s begin", url);
    Close();
    demuxMutex.lock();

    int re = avformat_open_input(&ic, url, nullptr, nullptr);
    if (re != 0) {
        demuxMutex.unlock();
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf));
        LOGE("FFDemux Open File %s failed! -- %s", url, buf);
        return false;
    }

    LOGI("FFDemux Open File %s Success!", url);

    // 读取文件信息
    re = avformat_find_stream_info(ic, nullptr);
    if (re != 0) {
        demuxMutex.unlock();
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf));
        LOGE("FFDemux find stream info %s failed! -- %s", url, buf);
        return false;
    }

    this->totalMs = ic->duration / (AV_TIME_BASE / 1000);
    demuxMutex.unlock();

    LOGI("%s total ms = %d ！", url, totalMs);

    GetVideoParam();
    GetAudioParam();

    return true;
}

bool FFDemux::Seek(double pos) {
    if (pos < 0 || pos > 1) {
        LOGE("seek value must 0.0 ~ 1.0");
        return false;
    }

    bool re = false;
    demuxMutex.lock();
    if (!ic) {
        demuxMutex.unlock();
        LOGE("GetVideoParam failed. ic is null!!!");
        return re;
    }

    avformat_flush(ic);
    double seekPts = 0;
    seekPts = ic->streams[videoStreamIndex]->duration * pos;

    //往后跳转到关键帧
    re = av_seek_frame(ic, videoStreamIndex, seekPts, AVSEEK_FLAG_FRAME | AVSEEK_FLAG_BACKWARD);
    demuxMutex.unlock();
    return re;
}

void FFDemux::Close() {
    demuxMutex.lock();
    if (ic) {
        avformat_close_input(&ic);
    }
    demuxMutex.unlock();
}

PlayParams FFDemux::GetVideoParam() {
    PlayParams params;
    demuxMutex.lock();
    if (!ic) {
        demuxMutex.unlock();
        LOGE("GetVideoParam failed. ic is null!!!");
        return params;
    }

    int re = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (re < 0) {
        demuxMutex.unlock();
        LOGE("could not find video stream in %s.", ic->url);
        return params;
    }

    videoStreamIndex = re;
    params.param = ic->streams[videoStreamIndex]->codecpar;
    demuxMutex.unlock();
    return params;
}

PlayParams FFDemux::GetAudioParam() {
    PlayParams params;

    demuxMutex.lock();
    if (!ic) {
        demuxMutex.unlock();
        LOGE("GetVideoParam failed. ic is null!!!");
        return params;
    }

    int index = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (index < 0) {
        demuxMutex.unlock();
        LOGE("Could not find audio stream in %s.", ic->url);
        return params;
    }

    audioStreamIndex = index;
    params.param = ic->streams[index]->codecpar;
    demuxMutex.unlock();

    return params;
}

PlayData FFDemux::Read() {
    demuxMutex.lock();
    if (!ic) {
        demuxMutex.unlock();
        return PlayData();
    }

    AVPacket *packet = av_packet_alloc();
    int re = av_read_frame(ic, packet);
    if (re != 0) {
        demuxMutex.unlock();
        av_packet_free(&packet);
        return PlayData();
    }
    PlayData playData;
    if (packet->stream_index == videoStreamIndex) {
        playData.isAudio = false;
    } else if (packet->stream_index == audioStreamIndex) {
        playData.isAudio = true;
    } else {
        // 忽略其他类型数据
        demuxMutex.unlock();
        av_packet_free(&packet);
        return PlayData();
    }

    playData.data = (unsigned char *) packet;
    playData.size = packet->size;

    // 转换pts
    packet->pts = packet->pts * (1000 * r2d(ic->streams[packet->stream_index]->time_base));
    packet->dts = packet->dts * (1000 * r2d(ic->streams[packet->stream_index]->time_base));
    playData.pts = (int) packet->pts;

    // LOGI("demux pts %d.", playData.pts);

    demuxMutex.unlock();
    return playData;
}
