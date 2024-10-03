#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>

using namespace geode::prelude;

class $modify(MyLevelSearchLayer, LevelSearchLayer) {

    static void onModify(auto& self) {
        (void) self.setHookPriority("LevelSearchLayer::init", INT_MIN);
    }

    bool init(int p0) {
        if (!LevelSearchLayer::init(p0)) return false;
        if (UIModding::get()->doModify) {
            UIModding::get()->updateColors(this, "level-search-bg");
            UIModding::get()->updateColors(this, "level-search-bar-bg");
            UIModding::get()->updateColors(this, "quick-search-bg");
            UIModding::get()->updateColors(this, "difficulty-filters-bg");
            UIModding::get()->updateColors(this, "length-filters-bg");
        }
        return true;
    }
};