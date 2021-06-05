/**
 * opengl shader封装
 */

#ifndef LOSERPLAYER_SHADER_H
#define LOSERPLAYER_SHADER_H

#include <mutex>

enum ShaderType {
    SHADER_YUV420P = 0,
    SHADER_NV12 = 25,
    SHADER_NV21 = 26
};

class Shader {
public:
    virtual bool Init(ShaderType type = SHADER_YUV420P);

    virtual void Close();

    virtual void
    GetTexture(unsigned int index, int width, int height, unsigned char *buf, bool isa = false);

    virtual void Draw();

protected:
    unsigned int vertexShader = 0;
    unsigned int fragmentShader = 0;
    unsigned int program = 0;
    unsigned int textures[100] = {0};
    std::mutex mutex;
};


#endif //LOSERPLAYER_SHADER_H
