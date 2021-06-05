#include "Texture.h"
#include "Shader.h"
#include "EGL.h"
#include <AndroidLogger.h>
#include <GLES2/gl2.h>

class CTexture : public Texture {
public:
    Shader shader;
    TextureType type;
    std::mutex mutex;

    bool Init(void *win, TextureType textureType) override {
        mutex.lock();

        EGL::Get()->Close();

        shader.Close();

        this->type = textureType;

        if (!win) {
            mutex.unlock();
            LOGE("texture init failed, win is null!!!");
            return false;
        }

        if (!EGL::Get()->Init(win)) {
            mutex.unlock();
            LOGE("egl init failed!!!");
            return false;
        }

        shader.Init(static_cast<ShaderType>(textureType));

        mutex.unlock();

        return true;
    };

    void Draw(unsigned char *data[], int width, int height) override {
        mutex.lock();

        // glViewport(0, 0, width, height);

        shader.GetTexture(0, width, height, data[0]);

        if (type == TEXTURE_YUV420P) {
            shader.GetTexture(1, width / 2, height / 2, data[1]);
            shader.GetTexture(2, width / 2, height / 2, data[2]);
        } else {
            shader.GetTexture(1, width / 2, height / 2, data[1], true);
        }

        shader.Draw();

        EGL::Get()->Draw();
        mutex.unlock();
    };

    void Drop() override {
        mutex.lock();
        EGL::Get()->Close();
        shader.Close();
        mutex.unlock();
        delete this;
    };
};

Texture *Texture::Create() {
    return new CTexture();
}
