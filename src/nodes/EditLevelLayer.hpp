
#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include "../Utils.hpp"
#include "../Macros.hpp"

using namespace geode::prelude;

class $modify(MyEditLevelLayer, EditLevelLayer) {

    static void onModify(auto& self) {
        HOOK_LATEST("EditLevelLayer::init");
    }

    bool init(GJGameLevel* p0) {
        if (!EditLevelLayer::init(p0)) return false;
        if (UIModding::get()->doModify) {
            if (CCScale9Sprite* bg = typeinfo_cast<CCScale9Sprite*>(this->getChildByID("description-background"))) {
                Utils::setColorIfExists(bg, "edit-description-bg");
            }
            if (CCScale9Sprite* bg = typeinfo_cast<CCScale9Sprite*>(this->getChildByID("level-name-background"))) {
                Utils::setColorIfExists(bg, "edit-name-bg");
            }
        }
        return true;
    }
};