/**
 * 播放器代理
 */

#ifndef LOSERPLAYER_IPLAYERPROXY_H
#define LOSERPLAYER_IPLAYERPROXY_H


#include "IPlayer.h"
#include <mutex>

class IPlayerProxy : public IPlayer {
public:
    static IPlayerProxy *Get() {
        static IPlayerProxy proxy;
        return &proxy;
    }

    void Init(void *vm = 0);

    bool Open(const char *url) override;

    void Close() override;

    bool Start() override;

    void InitView(void *win) override;

    double GetPlayProgress() override;

    bool Seek(double pos) override;

    void Pause(bool pause) override;

    bool IsPause() override;

protected:
    void Run() override {
        IPlayer::Run();
    }

protected:
    IPlayerProxy() {}

    IPlayer *player = 0;
    std::mutex mux;
};

#endif //LOSERPLAYER_IPLAYERPROXY_H
