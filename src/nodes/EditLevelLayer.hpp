
#pragma once

#include <Geode/Geode.hpp>
#include "../Utils.hpp"
#include "../Macros.hpp"
#include <alphalaneous.alphas_geode_utils/include/NodeModding.h>

using namespace geode::prelude;

class $nodeModify(MyEditLevelLayer, EditLevelLayer) {

    void modify() {
        if (UIModding::get()->doModify) {
            if (CCScale9Sprite* bg = typeinfo_cast<CCScale9Sprite*>(this->getChildByID("description-background"))) {
                Utils::setColorIfExists(bg, "edit-description-bg");
            }
            if (CCScale9Sprite* bg = typeinfo_cast<CCScale9Sprite*>(this->getChildByID("level-name-background"))) {
                Utils::setColorIfExists(bg, "edit-name-bg");
            }
        }
    }
};