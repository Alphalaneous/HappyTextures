#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCDirector.hpp>
#include <Geode/modify/LeaderboardsLayer.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include "UIModding.hpp"
#include "Macros.hpp"
#include "Utils.hpp"

using namespace geode::prelude;

void setBackground(CCNode* node) {
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

class $modify(LeaderboardsLayer) {

    static void onModify(auto& self) {
        HOOK_LATEST("LeaderboardsLayer::init");
    }

    bool init(LeaderboardState p0) {
        if (!LeaderboardsLayer::init(p0)) return false;
        setBackground(this);
        return true;
    }
};

class $modify(LevelBrowserLayer) {

    static void onModify(auto& self) {
        HOOK_LATEST("LevelBrowserLayer::init");
    }

    bool init(GJSearchObject* p0) {
        if (!LevelBrowserLayer::init(p0)) return false;
        setBackground(this);
        return true;
    }
};