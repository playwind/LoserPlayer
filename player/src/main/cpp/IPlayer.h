/**
 * 播放器接口类
 */

#ifndef LOSERPLAYER_IPLAYER_H
#define LOSERPLAYER_IPLAYER_H

#include <mutex>
#include "Thread.h"
#include "PlayParams.h"

class IDemux;

class IAudioPlay;

class IVideoView;

class IResample;

class IDecode;

class IPlayer : public Thread {
public:
    IDemux *demux = nullptr;
    IDecode *videoDecode = nullptr;
    IDecode *audioDecode = nullptr;
    IResample *resample = nullptr;
    IVideoView *videoView = nullptr;
    IAudioPlay *audioPlay = nullptr;

    PlayParams outParams;
    
    bool isHardDecode = false;

    static IPlayer *Get(unsigned char index = 0);

    virtual bool Open(const char *url);

    virtual void Close();

    virtual bool Start();

    virtual void InitView(void *win);

    virtual double GetPlayProgress();

    virtual bool Seek(double pos);

    virtual void Pause(bool pause);

protected:
    // 用于音视频同步
    void Run();

    std::mutex mux;

    IPlayer() {};
};


#endif //LOSERPLAYER_IPLAYER_H
