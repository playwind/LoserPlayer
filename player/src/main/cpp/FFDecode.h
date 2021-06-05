/**
 * 解码器FFmpeg实现
 */

#ifndef LOSERPLAYER_FFDECODE_H
#define LOSERPLAYER_FFDECODE_H

#include "IDecode.h"
#include "PlayParams.h"

struct AVCodecContext;
struct AVFrame;

class FFDecode : public IDecode {
public:
    static void InitHardDecode(void *vm);

    virtual bool Open(PlayParams params, bool isHardDecode = false);

    virtual void Close();

    virtual void Clear();

    virtual bool SendPacket(PlayData playData);

    virtual PlayData RecvFrame();

protected:
    AVCodecContext *codecCtx = nullptr;
    AVFrame *frame = nullptr;
    std::mutex mux;
};


#endif //LOSERPLAYER_FFDECODE_H
