/**
 * 音频播放OpenSLES实现
 */

#ifndef LOSERPLAYER_SLAUDIOPLAY_H
#define LOSERPLAYER_SLAUDIOPLAY_H


#include "IAudioPlay.h"

class SLAudioPlay : public IAudioPlay {
public:
    SLAudioPlay();

    virtual ~SLAudioPlay();

    virtual bool StartPlay(PlayParams params);

    virtual void Close();

    void PlayCall(void *bufq);

protected:
    unsigned char *buf = 0;
    std::mutex audioMutex;
};


#endif //LOSERPLAYER_SLAUDIOPLAY_H
