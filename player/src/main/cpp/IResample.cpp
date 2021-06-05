#include "IResample.h"
#include <AndroidLogger.h>

void IResample::Update(PlayData data) {
    PlayData output = this->Resample(data);
    // LOGI("resample data: %d.", output.size);
    if (output.size > 0) {
        this->Notify(output);
    }
}
