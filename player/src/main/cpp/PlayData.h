/**
 * 播放数据封装()
 */

#ifndef LOSERPLAYER_PLAYDATA_H
#define LOSERPLAYER_PLAYDATA_H

enum PlayDataType {
    TYPE_AV_PACKET = 0,
    TYPE_UCHAR = 1
};

struct PlayData {
    int type = TYPE_AV_PACKET;
    int pts = 0;
    unsigned char *data = nullptr;
    unsigned char *datas[8] = {nullptr};
    int size = 0;
    bool isAudio = false;
    int width = 0;
    int height = 0;
    int format = 0;

    bool Alloc(int size, const char *data = 0);

    void Drop();
};


#endif //LOSERPLAYER_PLAYDATA_H
