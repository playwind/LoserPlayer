#include "IPlayerBuilder.h"
#include "IDemux.h"
#include "IDecode.h"
#include "IResample.h"
#include "IAudioPlay.h"
#include "IVideoView.h"

IPlayer *IPlayerBuilder::BuildPlayer(unsigned char index) {
    IPlayer *player = CreatePlayer(index);

    IDemux *demux = CreateDemux();
    IDecode *videoDecode = CreateDecode();
    IDecode *audioDecode = CreateDecode();

    demux->AddObserver(videoDecode);
    demux->AddObserver(audioDecode);

    IVideoView *videoView = CreateVideoView();
    videoDecode->AddObserver(videoView);

    IResample *resample = CreateResample();
    audioDecode->AddObserver(resample);

    IAudioPlay *audioPlay = CreateAudioPlay();
    resample->AddObserver(audioPlay);

    player->demux = demux;
    player->videoDecode = videoDecode;
    player->audioDecode = audioDecode;
    player->resample = resample;
    player->videoView = videoView;
    player->audioPlay = audioPlay;

    return player;
}
