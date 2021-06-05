/**
 * 音频重采样接口类
 */

#ifndef LOSERPLAYER_IRESAMPLE_H
#define LOSERPLAYER_IRESAMPLE_H


#include "IObserver.h"
#include "PlayParams.h"

class IResample : public IObserver {
public:
    virtual bool Open(PlayParams in, PlayParams out = PlayParams()) = 0;

    virtual PlayData Resample(PlayData inData) = 0;

    virtual void Close() = 0;

    virtual void Update(PlayData data);

    int outChannels = 2;
    int outFormat = 1;
};


#endif //LOSERPLAYER_IRESAMPLE_H
