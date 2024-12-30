#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCDirector.hpp>
#include "../UIModding.h"
#include "../Utils.h"
#include "../Macros.h"

using namespace geode::prelude;

class $modify(MyCCDirector, CCDirector) {

    void willSwitchToScene(CCScene* scene) {
        CCDirector::willSwitchToScene(scene);

        for (CCNode* node : CCArrayExt<CCNode*>(scene->getChildren())) {
            UIModding::get()->doUICheck(node, true);
        }
    }
};
