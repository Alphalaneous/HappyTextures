#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCObject.hpp>
#include <Geode/modify/CCPoolManager.hpp>
#include "UIModding.hpp"
#include "nodes/CCNode.hpp"
#include "Utils.hpp"
#include "Macros.hpp"

using namespace geode::prelude;

class LateQueue {
protected:
    static LateQueue* instance;
public:
    std::vector<std::function<void(void)>> m_mainThreadQueue;
    mutable std::mutex m_mainThreadMutex;

    void queue(ScheduledFunction&& func) {
        std::lock_guard<std::mutex> lock(m_mainThreadMutex);
        m_mainThreadQueue.push_back(std::forward<ScheduledFunction>(func));
    }

    void executeQueue() {
        m_mainThreadMutex.lock();
        auto queue = m_mainThreadQueue;
        m_mainThreadQueue.clear();
        m_mainThreadMutex.unlock();

        for (auto const& func : queue) {
            func();
        }
    }

    static LateQueue* get() {
        if (!instance) {
            instance = new LateQueue();
        };
        return instance;
    }
};
LateQueue* LateQueue::instance = nullptr;

class $modify(CCObject) {

    CCObject* autorelease() {
        auto modding = UIModding::get();
        if (!modding->doModify) return CCObject::autorelease();
        
        if (MyCCNode* node = static_cast<MyCCNode*>(typeinfo_cast<CCNode*>(this))) {
            if (modding->finishedLoad && !node->isModified()) {
                const std::string& className = Utils::getNodeName(node);
                node->retain();
                LateQueue::get()->queue([modding, node, className] {
                    modding->doUICheckForType(className, node);
                    node->setModified();
                    node->release();
                });
            }
        }
        
        return CCObject::autorelease();
    }
};

class $modify(CCPoolManager) {
    void pop() {
        LateQueue::get()->executeQueue();
        CCPoolManager::pop();
    }
};