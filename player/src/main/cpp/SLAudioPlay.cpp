#include "SLAudioPlay.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <AndroidLogger.h>

static SLObjectItf engineSl = nullptr;
static SLEngineItf eng = nullptr;
static SLObjectItf mix = nullptr;
static SLObjectItf player = nullptr;
static SLPlayItf iPlayer = nullptr;
static SLAndroidSimpleBufferQueueItf pcmQue = nullptr;

static SLEngineItf CreateSL() {
    SLresult re;
    SLEngineItf engineItf;
    re = slCreateEngine(&engineSl, 0, 0, 0, 0, 0);
    if (re != SL_RESULT_SUCCESS) return nullptr;

    re = (*engineSl)->Realize(engineSl, SL_BOOLEAN_FALSE);
    if (re != SL_RESULT_SUCCESS) return nullptr;

    re = (*engineSl)->GetInterface(engineSl, SL_IID_ENGINE, &engineItf);
    if (re != SL_RESULT_SUCCESS) return nullptr;

    return engineItf;
}

static void PcmCall(SLAndroidSimpleBufferQueueItf bf, void *context) {
    if (!context) {
        LOGE("PcmCall Failed, context is null!!!");
        return;
    }
    auto *audioPlay = (SLAudioPlay *) context;
    audioPlay->PlayCall((void *) bf);
}

SLAudioPlay::SLAudioPlay() {
    buf = new unsigned char[1024 * 1024];
}

SLAudioPlay::~SLAudioPlay() {
    delete buf;
    buf = nullptr;
}

void SLAudioPlay::PlayCall(void *bufq) {
    // LOGI("PlayCall");
    if (!bufq) return;

    audioMutex.lock();

    auto bf = (SLAndroidSimpleBufferQueueItf) bufq;

    PlayData d = GetData();

    if (d.size <= 0) {
        LOGE("GetData size is 0");
        audioMutex.unlock();
        return;
    }

    memcpy(buf, d.data, d.size);

    if (pcmQue && (*pcmQue)) {
        (*pcmQue)->Enqueue(pcmQue, buf, d.size);
        // LOGI("OpenSL Enqueue data, size = %d.", d.size);
    }

     d.Drop();

    audioMutex.unlock();
}

bool SLAudioPlay::StartPlay(PlayParams params) {
    Close();
    audioMutex.lock();

    // 1. 创建引擎
    eng = CreateSL();
    if (eng) {
        LOGI("Create SL engine Success.");
    } else {
        audioMutex.unlock();
        LOGE("Create SL engine failed!!!");
        return false;
    }

    // 2. 创建混音器
    SLresult re = 0;
    re = (*eng)->CreateOutputMix(eng, &mix, 0, 0, 0);
    if (re != SL_RESULT_SUCCESS) {
        audioMutex.unlock();
        LOGE("CreateOutputMix Failed!!!");
        return false;
    }

    re = (*mix)->Realize(mix, SL_BOOLEAN_FALSE);
    if (re != SL_RESULT_SUCCESS) {
        audioMutex.unlock();
        LOGE("Mix Realize Failed!!!");
        return false;
    }

    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, mix};
    SLDataSink audioSink = {&outputMix, 0};

    // 3. 配置音频信息
    // 缓冲队列
    SLDataLocator_AndroidSimpleBufferQueue queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 10};
    // 音频格式
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,
            (SLuint32) params.channels,
            (SLuint32) params.sample_rate * 1000,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN
    };
    SLDataSource ds = {&queue, &pcm};

    // 4. 创建播放器
    const SLInterfaceID ids[] = {SL_IID_BUFFERQUEUE};
    const SLboolean req[] = {SL_BOOLEAN_TRUE};
    re = (*eng)->CreateAudioPlayer(eng, &player, &ds, &audioSink,
                                   sizeof(ids) / sizeof(SLInterfaceID), ids, req);
    if (re != SL_RESULT_SUCCESS) {
        audioMutex.unlock();
        LOGE("SL CreateAudioPlayer failed!!!");
        return false;
    } else {
        LOGI("SL CreateAudioPlayer success.");
    }

    (*player)->Realize(player, SL_BOOLEAN_FALSE);
    // 获取player接口
    re = (*player)->GetInterface(player, SL_IID_PLAY, &iPlayer);
    if (re != SL_RESULT_SUCCESS) {
        audioMutex.unlock();
        LOGE("SL Player GetInterface failed!!!");
        return false;
    }

    re = (*player)->GetInterface(player, SL_IID_BUFFERQUEUE, &pcmQue);
    if (re != SL_RESULT_SUCCESS) {
        audioMutex.unlock();
        LOGE("SL Player SL_IID_BUFFERQUEUE failed!!!");
        return false;
    }

    // 设置回调函数, 播放队列调用
    (*pcmQue)->RegisterCallback(pcmQue, PcmCall, this);

    // 设置为播放状态
    (*iPlayer)->SetPlayState(iPlayer, SL_PLAYSTATE_PLAYING);

    // 启动队列回调
    (*pcmQue)->Enqueue(pcmQue, "", 1);

    isExit = false;
    audioMutex.unlock();
    return true;
}

void SLAudioPlay::Close() {
    IAudioPlay::Clear();
    audioMutex.lock();

    // 停止播放
    if (iPlayer && (*iPlayer)) {
        (*iPlayer)->SetPlayState(iPlayer, SL_PLAYSTATE_STOPPED);
    }

    // 清理播放队列
    if (pcmQue && (*pcmQue)) {
        (*pcmQue)->Clear(pcmQue);
    }

    // 销毁player对象
    if (player && (*player)) {
        (*player)->Destroy(player);
    }

    // 销毁混音器
    if (mix && (*mix)) {
        (*mix)->Destroy(mix);
    }

    // 销毁播放引擎
    if (engineSl && (*engineSl)) {
        (*engineSl)->Destroy(engineSl);
    }

    engineSl = nullptr;
    eng = nullptr;
    mix = nullptr;
    player = nullptr;
    iPlayer = nullptr;
    pcmQue = nullptr;

    audioMutex.unlock();
}
