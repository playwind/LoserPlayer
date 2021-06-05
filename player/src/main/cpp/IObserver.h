/**
 * 观察者模式
 */

#ifndef LOSERPLAYER_IOBSERVER_H
#define LOSERPLAYER_IOBSERVER_H

#include "Thread.h"
#include "PlayData.h"
#include <vector>
#include <mutex>

class IObserver : public Thread {
public:
    // 观察者接收数据函数
    virtual void Update(PlayData data) {}

    // 添加观察者(线程安全)
    void AddObserver(IObserver *observer);

    // 通知所有观察者(线程安全)
    void Notify(PlayData data);

protected:
    std::vector<IObserver *> observers;
    std::mutex mutex;
};


#endif //LOSERPLAYER_IOBSERVER_H
