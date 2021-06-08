/**
 * 音频播放接口类
 */

#ifndef LOSERPLAYER_IAUDIOPLAY_H
#define LOSERPLAYER_IAUDIOPLAY_H


#include "IObserver.h"
#include "PlayParams.h"
#include <list>

class IAudioPlay : public IObserver {
public:
    // 缓冲满后阻塞
    virtual void Update(PlayData data);

    // 获取缓冲数据，没有则阻塞
    virtual PlayData GetData();

    virtual bool StartPlay(PlayParams out) = 0;

    virtual void Close() = 0;

    virtual void Clear();

    // 最大缓冲帧
    int maxFrame = 300;
    int pts = 0;
protected:
    std::list<PlayData> frames;
    std::mutex frameMutex;
};


#endif //LOSERPLAYER_IAUDIOPLAY_H
