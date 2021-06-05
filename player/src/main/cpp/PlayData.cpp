#include "PlayData.h"
#include <AndroidLogger.h>
extern "C" {
#include <libavformat/avformat.h>
}

bool PlayData::Alloc(int size, const char *d) {
    Drop();

    type = TYPE_UCHAR;

    if (size <= 0) return false;

    this->data = new unsigned char[size];

    if (!this->data) return false;

    if (d) {
        memcpy(this->data, d, size);
    }

    this->size = size;
    return true;
}

void PlayData::Drop() {
    if (!data) return;

    // LOGI("PlayData::Drop type = %d, size = %d", type, size);

    if (type == TYPE_AV_PACKET) {
        av_packet_free(reinterpret_cast<AVPacket **>(&data));
    } else {
        delete data;
    }

    data = nullptr;
    size = 0;
}
