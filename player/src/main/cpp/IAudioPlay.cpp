#include "IAudioPlay.h"

void IAudioPlay::Update(PlayData data) {
    // 压入缓冲队列
    if (data.size <= 0 || !data.data) return;
    while (!isExit) {
        frameMutex.lock();
        if (frames.size() > maxFrame) {
            frameMutex.unlock();
            XSleep(1);
            continue;
        }
        frames.push_back(data);
        frameMutex.unlock();
        break;
    }
}

PlayData IAudioPlay::GetData() {
    isRunning = true;
    PlayData data;
    while (!isExit) {
        if (IsPause()) {
            XSleep(2);
            continue;
        }

        frameMutex.lock();
        if (!frames.empty()) {
            data = frames.front();
            frames.pop_front();
            frameMutex.unlock();
            pts = data.pts;
            return data;
        }
        frameMutex.unlock();
        XSleep(1);
    }

    isRunning = false;

    return data;
}

void IAudioPlay::Clear() {
    frameMutex.lock();
    while (!frames.empty()) {
        frames.front().Drop();
        frames.pop_front();
    }
    frameMutex.unlock();
}
