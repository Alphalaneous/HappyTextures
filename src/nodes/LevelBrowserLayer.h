#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include "../UIModding.h"

using namespace geode::prelude;

class $modify(MyLevelBrowserLayer, LevelBrowserLayer) {

    struct Fields {
        ~Fields() {
            UIModding::get()->levelBrowserLayer = nullptr;
        }
    };

    static LevelBrowserLayer* get() {
        return UIModding::get()->levelBrowserLayer;
    }

    bool init(GJSearchObject* p0) {
        UIModding::get()->levelBrowserLayer = this;
        return LevelBrowserLayer::init(p0);
    }

};