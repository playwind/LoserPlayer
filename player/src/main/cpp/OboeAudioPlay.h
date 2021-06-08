#ifndef LOSERPLAYER_OBOEAUDIOPLAY_H
#define LOSERPLAYER_OBOEAUDIOPLAY_H

#include <oboe/Oboe.h>
#include "IAudioPlay.h"

class OboeAudioPlay
        : public IAudioPlay, oboe::AudioStreamDataCallback, oboe::AudioStreamErrorCallback {
public:
    virtual ~OboeAudioPlay() = default;

    virtual bool StartPlay(PlayParams out) override;

    virtual void Close() override;

    virtual oboe::DataCallbackResult onAudioReady(
            oboe::AudioStream *audioStream,
            void *audioData,
            int32_t numFrames) override;

    virtual bool onError(oboe::AudioStream *audioStream, oboe::Result error) override;

protected:
    std::mutex mLock;
    std::shared_ptr<oboe::AudioStream> mStream;
};


#endif //LOSERPLAYER_OBOEAUDIOPLAY_H
