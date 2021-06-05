/**
 * C++11 线程库封装
 */

#ifndef LOSERPLAYER_THREAD_H
#define LOSERPLAYER_THREAD_H


// sleep 毫秒
void XSleep(int milliseconds);

class Thread {
public:
    virtual bool Start();

    virtual void Stop();

    virtual void Pause(bool pause);

    virtual bool IsPause() {
        isPausing = isPause;
        return isPause;
    }

    virtual void Run() {}

protected:
    bool isExit = false;
    bool isRunning = false;
    bool isPause = false;
    bool isPausing = false;
private:
    void ThreadMain();
};

#endif //LOSERPLAYER_THREAD_H
