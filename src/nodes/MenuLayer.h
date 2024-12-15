#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

class $modify(MyMenuLayer, MenuLayer) {

    static void onModify(auto& self) {
        (void) self.setHookPriority("MenuLayer::init", INT_MIN/2-1);
    }

    bool init() {
        UIModding::get()->finishedLoad = true;
        if (!MenuLayer::init()) return false;
        UIModding::get()->doModify = Mod::get()->getSettingValue<bool>("ui-modifications");

        if (UIModding::get()->doModify) {
            if (Mod::get()->getSettingValue<bool>("hot-reload")) {
                UIModding::get()->startFileListeners();
            }
            UIModding::get()->doUICheck(this);
        }
        return true;
    }
};