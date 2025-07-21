#include "LateQueue.hpp"

LateQueue* LateQueue::instance = nullptr;

void LateQueue::queue(ScheduledFunction&& func) {
    m_mainThreadQueue.push_back(std::move(func));
}

void LateQueue::executeQueue() {
    for (auto const& func : m_mainThreadQueue) {
        if (func) func();
    }
    m_mainThreadQueue.clear();
}

LateQueue* LateQueue::get() {
    if (!instance) {
        instance = new LateQueue();
    };
    return instance;
}