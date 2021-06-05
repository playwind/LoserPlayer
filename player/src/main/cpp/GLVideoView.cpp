#include "GLVideoView.h"
#include <AndroidLogger.h>
#include "Texture.h"

void GLVideoView::SetRender(void *win) {
    this->view = win;
}

void GLVideoView::Render(PlayData data) {
    mux.lock();
    if (!view) return;
    if (!texture) {
        texture = Texture::Create();
        texture->Init(view, (TextureType) data.format);
    }

    texture->Draw(data.datas, data.width, data.height);
    mux.unlock();
}

void GLVideoView::Close() {
    mux.lock();
    if (texture) {
        texture->Drop();
        texture = nullptr;
    }
    mux.unlock();
}
