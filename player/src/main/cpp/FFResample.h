/**
 * 音频重采样FFmpeg实现
 */

#ifndef LOSERPLAYER_FFRESAMPLE_H
#define LOSERPLAYER_FFRESAMPLE_H


#include "IResample.h"

struct SwrContext;

class FFResample: public IResample {
public:
    virtual bool Open(PlayParams in, PlayParams out = PlayParams());

    virtual PlayData Resample(PlayData inData);

    virtual void Close();

protected:
    SwrContext *swrContext = nullptr;
    std::mutex mux;
};


#endif //LOSERPLAYER_FFRESAMPLE_H
