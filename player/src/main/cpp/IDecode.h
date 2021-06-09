#ifndef LOSERPLAYER_IDECODE_H
#define LOSERPLAYER_IDECODE_H

#include "PlayParams.h"
#include "IObserver.h"
#include <list>

class IDecode : public IObserver {
public:
    virtual bool Open(PlayParams params, bool isHardDecode = false) = 0;

    virtual void Close() = 0;

    virtual void Clear();

    // future模型, 发送数据到线程解码
    virtual bool SendPacket(PlayData playData) = 0;

    // 从线程中获取解码结果，再次调用会复用上次空间，线程不安全
    virtual PlayData RecvFrame() = 0;

    virtual void Update(PlayData playData);

    bool isAudio = false;

    // 最大缓冲帧数
    int maxList = 100;

    // 同步时间，再次打开文件要清理
    int syncPts = 0;
    int pts = 0;

protected:
    virtual void Run();

    // 读取缓冲
    std::list<PlayData> packs;
    std::mutex decodeMutex;
};


#endif //LOSERPLAYER_IDECODE_H
