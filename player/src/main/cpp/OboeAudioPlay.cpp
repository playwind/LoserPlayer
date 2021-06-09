#include "OboeAudioPlay.h"
#include <AndroidLogger.h>

bool OboeAudioPlay::StartPlay(PlayParams out) {
    LOGE("AudioPlay start, channels=%d, sampleRate=%d", out.channels, out.sample_rate);

    isPausing = isPause = isExit = false;
    mAudioParams = out;
    std::lock_guard<std::mutex> lock(mLock);
    oboe::AudioStreamBuilder builder;
    oboe::Result result = builder.setSharingMode(oboe::SharingMode::Exclusive)
            ->setDirection(oboe::Direction::Output)
            ->setPerformanceMode(oboe::PerformanceMode::LowLatency)
            ->setChannelCount(out.channels)
            ->setSampleRate(out.sample_rate)
            ->setSampleRateConversionQuality(oboe::SampleRateConversionQuality::Medium)
            ->setFormat(oboe::AudioFormat::I16)
            ->setDataCallback(this)
            ->setErrorCallback(this)
            ->setFramesPerDataCallback(1024)
            ->openStream(mStream);

    if (result != oboe::Result::OK) { ;
        LOGE("Failed to create stream. Error: %s", oboe::convertToText(result));
        return false;
    }

    // Typically, start the stream after querying some stream information, as well as some input from the user
    result = mStream->requestStart();

    if (result != oboe::Result::OK) { ;
        LOGE("Failed to requestStart. Error: %s", oboe::convertToText(result));
        return false;
    }

    return true;
}

void OboeAudioPlay::Close() {
    std::lock_guard<std::mutex> lock(mLock);
    Thread::Stop();
    if (mStream) {
        mStream->stop();
        mStream->close();
        mStream.reset();
    }
}

oboe::DataCallbackResult
OboeAudioPlay::onAudioReady(oboe::AudioStream *audioStream, void *audioData, int32_t numFrames) {
    if (!mStream) return oboe::DataCallbackResult::Stop;

    // LOGI("onAudioReady: frameSize = %d", numFrames);

    auto *buf = (unsigned char *) audioData;

    // buf大小=帧数*声道数*采样大小
    memset(buf, 0, numFrames * mAudioParams.channels * 2);

    if (IsPause()) {
        return oboe::DataCallbackResult::Continue;
    }

    // 这里取出来一共4096个字节=1024个音频帧
    PlayData d = GetData();

    // LOGE("onAudioReady GetData size is %d", d.size);

    if (d.size > 0) {
        memcpy(buf, d.data, d.size);
    }

    d.Drop();

    return oboe::DataCallbackResult::Continue;
}

bool OboeAudioPlay::onError(oboe::AudioStream *stream, oboe::Result error) {
    LOGE("Oboe onError: %s", oboe::convertToText(error));
    return true;
}
