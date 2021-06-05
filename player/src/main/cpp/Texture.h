/**
 * OpenGL纹理封装
 */

#ifndef LOSERPLAYER_TEXTURE_H
#define LOSERPLAYER_TEXTURE_H

enum TextureType {
    TEXTURE_YUV420P = 0,
    TEXTURE_NV12 = 25,
    TEXTURE_NV21 = 26
};

class Texture {
public:
    static Texture *Create();

    virtual bool Init(void *win, TextureType type = TEXTURE_YUV420P) = 0;

    virtual void Draw(unsigned char *data[], int width, int height) = 0;

    virtual void Drop() = 0;

    virtual ~Texture() {};
protected:
    Texture() {};
};


#endif //LOSERPLAYER_TEXTURE_H
