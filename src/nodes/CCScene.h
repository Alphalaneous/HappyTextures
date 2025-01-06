#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCScene.hpp>
#include "../UIModding.h"
#include "../Macros.h"
#include "../Callbacks.h"

using namespace geode::prelude;

class SceneHandler : public CCObject {
public:
    static SceneHandler* create() {
        auto ret = new SceneHandler();
        ret->autorelease();
        return ret;
    }

    CCScene* m_currentScene = nullptr;

    // IMPORTANT: might contain dangling pointers, 
    // do not use for anything other than checking
    std::unordered_set<CCNode*> m_handledNodes; 

    void checkForUpdates(CCScene* scene) {
        if (Callbacks::get()->m_ignoreUICheck) return;

        if (scene != m_currentScene) {
            // we're in a new scene, clear the handled nodes
            m_handledNodes.clear();
            m_currentScene = scene;
        }

        for (auto node : CCArrayExt<CCNode*>(scene->getChildren())) {
            if (m_handledNodes.find(node) != m_handledNodes.end()) continue;
            m_handledNodes.insert(node);

            if (node->getID() == "MenuLayer") continue; // hardcoded for now

            UIModding::get()->doUICheck(node);
        }
    }

    void update(float dt) {
        auto scene = CCDirector::sharedDirector()->getRunningScene();

        if (scene && UIModding::get()->doModify) {
            this->checkForUpdates(scene);
        }
    }
};

$execute {
    Loader::get()->queueInMainThread([]{
        CCScheduler::get()->scheduleUpdateForTarget(SceneHandler::create(), INT_MAX, false);
    });
}