#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "../Callbacks.h"
#include "../Macros.h"
#include "../UIModding.h"

using namespace geode::prelude;

class $modify(MyMenuLayer, MenuLayer) {

    static void onModify(auto& self) {
        HOOK_LATEST("MenuLayer::init");
    }

    bool init() {
        UIModding::get()->finishedLoad = true;
        if (!MenuLayer::init()) return false;

        UIModding::get()->doModify = Mod::get()->getSettingValue<bool>("ui-modifications");
        Callbacks::get()->generateAll();

        if (UIModding::get()->doModify) {
            if (Mod::get()->getSettingValue<bool>("hot-reload")) {
                UIModding::get()->startFileListeners();
            }
        }

        return true;
    }
};