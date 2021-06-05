/**
 * 视频播放OpenGL ES实现
 */

#ifndef LOSERPLAYER_GLVIDEOVIEW_H
#define LOSERPLAYER_GLVIDEOVIEW_H

#include "PlayData.h"
#include "IVideoView.h"

class Texture;

class GLVideoView : public IVideoView {
public:
    virtual void SetRender(void *win);

    virtual void Render(PlayData data);

    virtual void Close();

protected:
    void *view = nullptr;
    Texture *texture = 0;
    std::mutex mux;
};


#endif //LOSERPLAYER_GLVIDEOVIEW_H
