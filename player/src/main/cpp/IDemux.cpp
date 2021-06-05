#include "IDemux.h"

void IDemux::Run() {
    while (!isExit) {
        if (IsPause()) {
            XSleep(2);
            continue;
        }

        PlayData d = Read();
        if (d.size > 0) {
            Notify(d);
        } else {
            XSleep(2);
        }
    }
}
