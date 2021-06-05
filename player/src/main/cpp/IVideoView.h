/**
 * 视频播放接口类
 */

#ifndef LOSERPLAYER_IVIDEOVIEW_H
#define LOSERPLAYER_IVIDEOVIEW_H


#include "IObserver.h"

class IVideoView : public IObserver {
public:
    virtual void SetRender(void *win) = 0;

    virtual void Render(PlayData data) = 0;

    virtual void Update(PlayData data);

    virtual void Close() = 0;
};

#endif //LOSERPLAYER_IVIDEOVIEW_H
