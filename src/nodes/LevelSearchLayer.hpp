#pragma once

#include <Geode/Geode.hpp>
#include "../Macros.hpp"
#include <alphalaneous.alphas_geode_utils/include/NodeModding.h>
#include "../UIModding.hpp"

using namespace geode::prelude;

class $nodeModify(MyLevelSearchLayer, LevelSearchLayer) {
    void modify() {
        if (UIModding::get()->doModify) {
            UIModding::get()->updateColors(this, "level-search-bg");
            UIModding::get()->updateColors(this, "level-search-bar-bg");
            UIModding::get()->updateColors(this, "quick-search-bg");
            UIModding::get()->updateColors(this, "difficulty-filters-bg");
            UIModding::get()->updateColors(this, "length-filters-bg");
        }
    }
};