#include "FFResample.h"
#include <AndroidLogger.h>

extern "C" {
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
}

bool FFResample::Open(PlayParams in, PlayParams out) {
    Close();
    mux.lock();
    swrContext = swr_alloc();
    swrContext = swr_alloc_set_opts(swrContext, av_get_default_channel_layout(out.channels),
                                    AV_SAMPLE_FMT_S16, out.sample_rate,
                                    av_get_default_channel_layout((in.param)->channels),
                                    (AVSampleFormat) in.param->format, in.param->sample_rate, 0, 0);
    int re = swr_init(swrContext);
    if (re != 0) {
        mux.unlock();
        LOGE("swr_init failed!!!");
        return false;
    } else {
        LOGI("swr_init success.");
    }

    outChannels = in.param->channels;
    outFormat = AV_SAMPLE_FMT_S16;

    mux.unlock();
    return true;
}

void FFResample::Close() {
    mux.lock();
    if (swrContext) {
        swr_free(&swrContext);
    }
    mux.unlock();
}

PlayData FFResample::Resample(PlayData inData) {
    if (inData.size <= 0 || !inData.data) return PlayData();

    mux.lock();
    if (!swrContext) {
        mux.unlock();
        return PlayData();
    }

    auto *frame = (AVFrame *) inData.data;

    PlayData outData;
    int outSize =
            outChannels * frame->nb_samples * av_get_bytes_per_sample((AVSampleFormat) outFormat);
    if (outSize <= 0) return PlayData();

    outData.Alloc(outSize);
    uint8_t *outArr[2] = {nullptr};
    outArr[0] = outData.data;
    int len = swr_convert(swrContext, outArr, frame->nb_samples, (const uint8_t **) frame->data,
                          frame->nb_samples);
    if (len <= 0) {
        mux.unlock();
        outData.Drop();
        return PlayData();
    }

    outData.pts = inData.pts;
    mux.unlock();

    return outData;
}
