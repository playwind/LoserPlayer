#include "EGL.h"
#include <android/native_window.h>
#include <EGL/egl.h>
#include <mutex>
#include <AndroidLogger.h>

class CEGL : public EGL {
public:
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;
    std::mutex mux;

    bool Init(void *win) override {
        LOGI("Init EGL win %d", win);

        auto *window = (ANativeWindow *) win;
        Close();
        mux.lock();
        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (display == EGL_NO_DISPLAY) {
            mux.unlock();
            LOGE("eglGetDisplay failed!!!");
            return false;
        }

        if (EGL_TRUE != eglInitialize(display, 0, 0)) {
            mux.unlock();
            LOGE("eglInitialize failed!!!");
            return false;
        }

        EGLint configSpec[] = {
                EGL_RED_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_BLUE_SIZE, 8,
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_NONE
        };

        EGLConfig config = 0;
        EGLint numConfigs = 0;
        if (EGL_TRUE != eglChooseConfig(display, configSpec, &config, 1, &numConfigs)) {
            mux.unlock();
            LOGE("eglChooseConfig failed!!!");
            return false;
        }

        surface = eglCreateWindowSurface(display, config, window, NULL);
        if (surface == EGL_NO_SURFACE) {
            mux.unlock();
            LOGE("eglCreateWindowSurface failed!!!");
            return false;
        }

        const EGLint ctxAttr[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
        context = eglCreateContext(display, config, EGL_NO_CONTEXT, ctxAttr);
        if (context == EGL_NO_CONTEXT) {
            mux.unlock();
            LOGE("eglCreateContext failed!!!");
            return false;
        }

        if (EGL_TRUE != eglMakeCurrent(display, surface, surface, context)) {
            mux.unlock();
            LOGE("eglMakeCurrent failed!!!");
            return false;
        }

        mux.unlock();
        return true;
    }

    void Close() override {
        mux.lock();
        if (display == EGL_NO_DISPLAY) {
            mux.unlock();
            return;
        }

        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        if (surface != EGL_NO_SURFACE) eglDestroySurface(display, surface);

        if (context != EGL_NO_CONTEXT) eglDestroyContext(display, context);

        eglTerminate(display);

        display = EGL_NO_DISPLAY;
        surface = EGL_NO_SURFACE;
        context = EGL_NO_CONTEXT;

        mux.unlock();
    }

    void Draw() override {
        mux.lock();
        if (display == EGL_NO_DISPLAY || surface == EGL_NO_SURFACE) {
            mux.unlock();
            return;
        }

        eglSwapBuffers(display, surface);

        mux.unlock();
    }
};

EGL *EGL::Get() {
    static CEGL egl;
    return &egl;
}
