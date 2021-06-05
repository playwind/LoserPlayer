#include "IPlayerProxy.h"
#include "FFPlayerBuilder.h"

void IPlayerProxy::Init(void *vm) {
    mux.lock();
    if (vm) {
        FFPlayerBuilder::InitHard(vm);
    }

    if (!player) {
        player = FFPlayerBuilder::Get()->BuildPlayer();
    }

    mux.unlock();
}


bool IPlayerProxy::Open(const char *url) {
    bool re = false;
    mux.lock();
    if (player) {
        player->isHardDecode = isHardDecode;
        re = player->Open(url);
    }
    mux.unlock();
    return re;
}

void IPlayerProxy::Close() {
    mux.lock();

    if (player) player->Close();

    mux.unlock();
}

bool IPlayerProxy::Start() {
    bool re = false;
    mux.lock();
    if (player) {
        re = player->Start();
    }
    mux.unlock();
    return re;
}

void IPlayerProxy::InitView(void *win) {
    mux.lock();
    if (player) player->InitView(win);
    mux.unlock();
}

double IPlayerProxy::GetPlayProgress() {
    double pos = 0.0;
    mux.lock();
    if (player) {
        pos = player->GetPlayProgress();
    }
    mux.unlock();
    return pos;
}

bool IPlayerProxy::Seek(double pos) {
    bool re = false;
    mux.lock();
    if (player) {
        re = player->Seek(pos);
    }
    mux.unlock();
    return re;
}

void IPlayerProxy::Pause(bool pause) {
    mux.lock();
    if (player) player->Pause(pause);
    mux.unlock();
}
