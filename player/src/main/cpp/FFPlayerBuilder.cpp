#include "FFPlayerBuilder.h"
#include "FFDecode.h"
#include "FFDemux.h"
#include "FFResample.h"
#include "GLVideoView.h"
#include "SLAudioPlay.h"
#include "OboeAudioPlay.h"

void FFPlayerBuilder::InitHard(void *vm) {
    FFDecode::InitHardDecode(vm);
}

IDemux *FFPlayerBuilder::CreateDemux() {
    return new FFDemux();
}

IDecode *FFPlayerBuilder::CreateDecode() {
    return new FFDecode();
}

IResample *FFPlayerBuilder::CreateResample() {
    return new FFResample();
}

IVideoView *FFPlayerBuilder::CreateVideoView() {
    return new GLVideoView();
}

IAudioPlay *FFPlayerBuilder::CreateAudioPlay() {
    // return new SLAudioPlay();
    return new OboeAudioPlay();
}

IPlayer *FFPlayerBuilder::CreatePlayer(unsigned char index) {
    return IPlayer::Get(index);
}
