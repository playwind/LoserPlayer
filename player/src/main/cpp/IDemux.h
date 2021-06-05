/**
 * 解封装接口类
 *
 */

#ifndef LOSERPLAYER_IDEMUX_H
#define LOSERPLAYER_IDEMUX_H

#include "IObserver.h"
#include "PlayParams.h"

class IDemux : public IObserver {
public:
    // 打开文件，流媒体
    virtual bool open(const char *url) = 0;

    // 跳转到指定位置 pos 0.0 - 1.0
    virtual bool Seek(double pos) = 0;

    // 关闭文件或流
    virtual void Close() = 0;

    // 获取视频参数
    virtual PlayParams GetVideoParam() = 0;

    // 获取音频参数
    virtual PlayParams GetAudioParam() = 0;

    // 读取一帧数据(由调用者清理)
    virtual PlayData Read() = 0;

    // 总时长(毫秒)
    int totalMs = 0;
protected:
    virtual void Run();
};

#endif //LOSERPLAYER_IDEMUX_H
