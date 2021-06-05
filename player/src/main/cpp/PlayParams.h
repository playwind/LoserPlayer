//
// Created by YQ18105 on 2021/6/2.
//

#ifndef LOSERPLAYER_PLAYPARAMS_H
#define LOSERPLAYER_PLAYPARAMS_H

struct AVCodecParameters;

class PlayParams {
public:
    AVCodecParameters *param = nullptr;
    int channels = 2;
    int sample_rate = 44100;
};


#endif //LOSERPLAYER_PLAYPARAMS_H
