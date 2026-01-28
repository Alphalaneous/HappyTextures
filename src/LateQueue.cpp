#include "LateQueue.hpp"

LateQueue* LateQueue::instance = nullptr;

void LateQueue::queue(CCObject* node, ScheduledFunction&& func) {
    m_mainThreadQueue[node].push_back(std::move(func));
}

void LateQueue::remove(CCObject* node) {
    m_mainThreadQueue.erase(node);
}

void LateQueue::executeQueue() {
    for (auto& [k, v] : m_mainThreadQueue) {
        for (auto& func : v) {
            if (func) func();
        }
    }
    m_mainThreadQueue.clear();
}

LateQueue* LateQueue::get() {
    if (!instance) {
        instance = new LateQueue();
    };
    return instance;
}