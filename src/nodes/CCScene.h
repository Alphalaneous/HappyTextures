#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCScene.hpp>
#include "../UIModding.h"
#include "../Macros.h"
#include "../Callbacks.h"

using namespace geode::prelude;

class $modify(MyCCScene, CCScene) {

    static void onModify(auto& self) {
        HOOK_LATEST("cocos2d::CCScene::create");
    }

    struct Fields {
        int m_currentCount = 0;
        bool m_isMenuLayer = false;
    };

    static CCScene* create() {
        auto ret = CCScene::create();
        if (UIModding::get()->doModify) {
            ret->schedule(schedule_selector(MyCCScene::checkForUpdates));
        }
        return ret;
    }

    void checkForUpdates(float dt) {
        if (this->getChildrenCount() != m_fields->m_currentCount && (this->getChildrenCount() != 1 || m_fields->m_currentCount == 0)) {
            int idx = 0;
           
            for (CCNode* node : CCArrayExt<CCNode*>(this->getChildren())) {
                idx++;
                if (node->getID() == "MenuLayer" || Callbacks::get()->m_ignoreUICheck) continue;
                if (idx > m_fields->m_currentCount) {
                    UIModding::get()->doUICheck(node);
                }
            }
        }
        m_fields->m_currentCount = this->getChildrenCount();
    }
};