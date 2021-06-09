/**
 * 解封装器，FFmpeg实现
 */

#ifndef LOSERPLAYER_FFDEMUX_H
#define LOSERPLAYER_FFDEMUX_H

#include "IDemux.h"
#include <mutex>

struct AVFormatContext;

class FFDemux : public IDemux {
public:
    virtual bool open(const char *url);

    virtual bool Seek(double pos);

    virtual void Close();

    virtual PlayParams GetVideoParam();

    virtual PlayParams GetAudioParam();

    virtual PlayData Read();

    char *GetStreamURL() override;

    FFDemux();

private:
    AVFormatContext *ic = nullptr;
    std::mutex demuxMutex;
    int videoStreamIndex = 0;
    int audioStreamIndex = 1;

};

#endif //LOSERPLAYER_FFDEMUX_H
