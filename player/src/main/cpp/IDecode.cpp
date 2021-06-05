#include "IDecode.h"
#include <AndroidLogger.h>

void IDecode::Run() {
    LOGE("IDecode Run, isAudio: %d" ,isAudio);
    while (!isExit) {
        if (IsPause()) {
            XSleep(2);
            continue;
        }

        decodeMutex.lock();

        // 判断音视频同步
        if (!isAudio && syncPts > 0) {
            if (syncPts < pts) {
                decodeMutex.unlock();
                XSleep(1);
                continue;
            }
        }

        if (packs.empty()) {
            decodeMutex.unlock();
            XSleep(1);
            continue;
        }

        // 取出packet消费
        PlayData playData = packs.front();
        packs.pop_front();

        // 发送数据到解码线程，一个数据包，可能解码多个结果
        if (this->SendPacket(playData)) {
            while (!isExit) {
                PlayData frame = RecvFrame();
                if (!frame.data) break;
                pts = frame.pts;
                this->Notify(frame);
            }
        }
        playData.Drop();
        decodeMutex.unlock();
    }
    LOGE("IDecode Run finished, isAudio: %d" ,isAudio);
}

void IDecode::Clear() {
    decodeMutex.lock();
    while (!packs.empty()) {
        packs.front().Drop();
        packs.pop_front();
    }
    pts = 0;
    syncPts = 0;
    decodeMutex.unlock();
}

void IDecode::Update(PlayData playData) {
    if (playData.isAudio != isAudio) return;

    while(!isExit) {
        decodeMutex.lock();
        if (packs.size() < maxList) {
            packs.push_back(playData);
            decodeMutex.unlock();
            break;
        }
        decodeMutex.unlock();
        XSleep(1);
    }
}
