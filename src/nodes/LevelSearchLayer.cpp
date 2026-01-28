#include <Geode/Geode.hpp>
#include "../Macros.hpp"
#include <alphalaneous.alphas_geode_utils/include/ObjectModify.hpp>
#include "../UIModding.hpp"

using namespace geode::prelude;

class $classModify(MyLevelSearchLayer, LevelSearchLayer) {
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