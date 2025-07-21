#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

//yoinked from geode
class LateQueue {
protected:
    static LateQueue* instance;
public:
    std::vector<ScheduledFunction> m_mainThreadQueue;

    void queue(ScheduledFunction&& func);
    void executeQueue();
    static LateQueue* get();
};