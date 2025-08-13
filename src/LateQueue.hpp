#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

//yoinked from geode
class LateQueue {
protected:
    static LateQueue* instance;
public:
    std::unordered_map<CCObject*, std::vector<ScheduledFunction>> m_mainThreadQueue;

    void queue(CCObject* node, ScheduledFunction&& func);
    void remove(CCObject* node);
    void executeQueue();
    static LateQueue* get();
};