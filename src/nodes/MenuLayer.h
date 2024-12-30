#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "../Callbacks.h"
#include "../Macros.h"

using namespace geode::prelude;

class $modify(MyMenuLayer, MenuLayer) {

    static void onModify(auto& self) {
        HOOK_LATEST("MenuLayer::init");
    }

    void onPlay(CCObject* obj) {
        if (CCNode* node = typeinfo_cast<CCNode*>(obj)) {
            if (node->getUserObject("dummy"_spr)) {
                CCDirector::get()->pushScene(CCTransitionFade::create(0.5, LevelSelectLayer::scene(0)));
                return;
            }
        }
        MenuLayer::onPlay(obj);
    }

    void onCreator(CCObject* obj) {
        if (CCNode* node = typeinfo_cast<CCNode*>(obj)) {
            if (node->getUserObject("dummy"_spr)) {
                CCDirector::get()->pushScene(CCTransitionFade::create(0.5, CreatorLayer::scene()));
                return;
            }
        }
        MenuLayer::onCreator(obj);
    }

    void onGarage(CCObject* obj) {
        if (CCNode* node = typeinfo_cast<CCNode*>(obj)) {
            if (node->getUserObject("dummy"_spr)) {
                CCDirector::get()->pushScene(CCTransitionFade::create(0.5, GJGarageLayer::scene()));
                return;
            }
        }
        MenuLayer::onGarage(obj);
    }

    bool init() {
        if (Callbacks::get()->m_ignoreUICheck) return MenuLayer::init();

        UIModding::get()->finishedLoad = true;
        if (!MenuLayer::init()) return false;
        UIModding::get()->doModify = Mod::get()->getSettingValue<bool>("ui-modifications");
        Callbacks::get()->generateAll();

        if (UIModding::get()->doModify) {
            if (Mod::get()->getSettingValue<bool>("hot-reload")) {
                UIModding::get()->startFileListeners();
            }
            UIModding::get()->doUICheck(this, UIModding::get()->initialScene);
            UIModding::get()->initialScene = false;
        }
        return true;
    }
};