#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCDirector.hpp>
#include <Geode/modify/LeaderboardsLayer.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include "UIModding.hpp"
#include "Macros.hpp"
#include "Utils.hpp"
#include <alphalaneous.alphas_geode_utils/include/NodeModding.h>

using namespace geode::prelude;

inline void setBackground(CCNode* node) {
    if (UIModding::get()->doModify) {
        if (CCSprite* bg = typeinfo_cast<CCSprite*>(node->getChildByIDRecursive("background"))) {
            if (bg->getColor() == ccColor3B{0, 102, 255}) {
                Utils::setColorIfExists(bg, "background");
            }
        }
    }
}

class $modify(CCDirector) {

    static void onModify(auto& self) {
        HOOK_LATEST("cocos2d::CCDirector::willSwitchToScene");
    }

    void willSwitchToScene(CCScene* scene) {
        setBackground(scene);
        CCDirector::willSwitchToScene(scene);
    }
};

class $nodeModify(LeaderboardsLayer) {
    void modify() {
        setBackground(this);
    }
};

class $nodeModify(LevelBrowserLayer) {
    void modify() {
        setBackground(this);
    }
};