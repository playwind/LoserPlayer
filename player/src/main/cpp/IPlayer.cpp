#include <AndroidLogger.h>
#include "IPlayer.h"
#include "IDemux.h"
#include "IDecode.h"
#include "IResample.h"
#include "IVideoView.h"
#include "IAudioPlay.h"

IPlayer *IPlayer::Get(unsigned char index) {
    static IPlayer player[256];
    return &player[index];
}

void IPlayer::Run() {
    while (!isExit) {
        mux.lock();

        if (!audioPlay || !videoDecode) {
            mux.unlock();
            XSleep(2);
            continue;
        }

        // 同步
        int audioPts = audioPlay->pts;
        videoDecode->syncPts = audioPts;

        mux.unlock();
        XSleep(2);
    }
}

bool IPlayer::Open(const char *url) {
    // Close();
    mux.lock();

    // 解封装
    if (!demux || !demux->open(url)) {
        mux.unlock();
        LOGE("player open file %s failed!!!", url);
        return false;
    }

    // 解码
    if (!videoDecode || !videoDecode->Open(demux->GetVideoParam(), isHardDecode)) {
        LOGE("video decoder open failed, hard decode = %d!!!", isHardDecode);

        // 解码不是必须的，如果解封装之后是原始数据那么就不需要解码
        // return false;
    }

    if (!audioDecode || !audioDecode->Open(demux->GetAudioParam())) {
        LOGE("audio decoder open failed");
    }

    // 音频重采样
    outParams = demux->GetAudioParam();
    if (!resample || !resample->Open(demux->GetAudioParam(), outParams)) {
        LOGE("resample open failed");
    }

    mux.unlock();
    return true;
}

void IPlayer::InitView(void *win) {
    if (videoView) {
        videoView->Close();
        videoView->SetRender(win);
    }
}

bool IPlayer::Start() {
    mux.lock();

    if (videoDecode) videoDecode->Start();

    if (!demux || !demux->Start()) {
        mux.unlock();
        LOGE("demux start failed!!!");
        return false;
    }

    if (audioDecode) audioDecode->Start();

    if (audioPlay) audioPlay->StartPlay(outParams);

    Thread::Start();

    mux.unlock();

    return true;
}

double IPlayer::GetPlayProgress() {
    double pos = 0.0;
    mux.lock();

    int total = 0;
    if (demux) total = demux->totalMs;

    if (total > 0) {
        if (videoDecode) {
            pos = (double) videoDecode->pts / (double) total;
        }
    }

    mux.unlock();
    return pos;
}

bool IPlayer::Seek(double pos) {
    bool re = false;
    if (!demux) return re;

    // 暂停所有线程
    Pause(true);

    mux.lock();

    // 清理缓冲队列
    if (videoDecode) videoDecode->Clear();
    if (audioDecode) audioDecode->Clear();
    if (audioPlay) audioPlay->Clear();

    re = demux->Seek(pos);

    if (!videoDecode) {
        mux.unlock();
        Pause(false);
        return re;
    }

    // 解码到实际需要显示的帧
    int seekPts = pos * demux->totalMs;
    while (!isExit) {
        PlayData packet = demux->Read();
        if (packet.size <= 0) break;

        if (packet.isAudio) {
            if (packet.pts < seekPts) {
                packet.Drop();
                continue;
            }

            // 写入缓冲队列
            demux->Notify(packet);
            continue;
        }

        videoDecode->SendPacket(packet);
        packet.Drop();
        PlayData data = videoDecode->RecvFrame();
        if (data.size <= 0) {
            continue;
        }
        if (data.pts >= seekPts) {
            break;
        }
    }

    mux.unlock();

    Pause(false);

    return true;
}

void IPlayer::Pause(bool pause) {
    mux.lock();

    Thread::Pause(pause);

    if (demux) demux->Pause(pause);

    if (videoDecode) videoDecode->Pause(pause);

    if (audioDecode) audioDecode->Pause(pause);

    if (audioPlay) audioPlay->Pause(pause);

    mux.unlock();
}

void IPlayer::Close() {
    mux.lock();
    Thread::Stop();

    if (demux) demux->Stop();
    if (audioDecode) audioDecode->Stop();
    if (videoDecode) videoDecode->Stop();
    if (audioPlay) audioPlay->Stop();

    if (videoDecode) videoDecode->Clear();
    if (audioDecode) audioDecode->Clear();
    if (audioPlay) audioPlay->Clear();

    if (audioPlay) audioPlay->Close();
    if (videoView) videoView->Close();
    if (videoDecode) videoDecode->Close();
    if (audioDecode) audioDecode->Close();
    if (demux) demux->Close();

    mux.unlock();
}

