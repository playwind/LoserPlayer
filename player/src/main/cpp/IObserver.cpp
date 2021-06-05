#include "IObserver.h"

void IObserver::AddObserver(IObserver *observer) {
    if (!observer) return;

    mutex.lock();
    observers.push_back(observer);
    mutex.unlock();
}

void IObserver::Notify(PlayData data) {
    mutex.lock();
    for (IObserver *observer : observers) {
        observer->Update(data);
    }
    mutex.unlock();
}
