#include "Thread.h"
#include <AndroidLogger.h>
#include <thread>

using namespace std;

void XSleep(int milliseconds) {
    chrono::milliseconds du(milliseconds);
    this_thread::sleep_for(du);
}

void Thread::ThreadMain() {
    isRunning = true;

    LOGI("线程%d开始运行", this_thread::get_id());

    Run();

    LOGI("线程%d运行结束", this_thread::get_id());

    isRunning = false;
}

bool Thread::Start() {
    isExit = false;
    isPause = false;
    thread th(&Thread::ThreadMain, this);
    th.detach();
    return true;
}

void Thread::Stop() {
    LOGI("Stop线程Begin!");

    isExit = true;

    for (int i = 0; i < 200; ++i) {
        if (!isRunning) {
            LOGI("Stop停止线程成功！");
            return;
        }

        XSleep(1);
    }

    LOGI("Stop停止线程超时！");
}

void Thread::Pause(bool pause) {
    isPause = pause;

    // 等待100毫秒
    for (int i = 0; i < 10; ++i) {
        if (isPausing == pause) {
            break;
        }

        XSleep(10);
    }
}





