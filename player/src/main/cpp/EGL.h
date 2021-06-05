/**
 * EGL封装
 */

#ifndef LOSERPLAYER_EGL_H
#define LOSERPLAYER_EGL_H


class EGL {
public:
    static EGL *Get();

    virtual bool Init(void *win) = 0;

    virtual void Close() = 0;

    virtual void Draw() = 0;

protected:
    EGL() {};
};


#endif //LOSERPLAYER_EGL_H
