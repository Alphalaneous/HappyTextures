#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/InfoLayer.hpp>

using namespace geode::prelude;

class $modify(MyInfoLayer, InfoLayer) {

    static void onModify(auto& self) {
        HOOK_LATEST("InfoLayer::init");
    }

    bool init(GJGameLevel* p0, GJUserScore* p1, GJLevelList* p2){
        if (!InfoLayer::init(p0, p1, p2)) return false;
        if (UIModding::get()->doModify) {
            CCNode* theNode;
            for (CCNode* node : CCArrayExt<CCNode*>(this->getChildren())) {
                if (node->getChildrenCount() > 1) {
                    theNode = node;
                    break;
                }
            }
            
            if (CCScale9Sprite* bg = typeinfo_cast<CCScale9Sprite*>(theNode->getChildByID("desc-background"))) {
                Utils::setColorIfExists(bg, "info-description-bg");
            }
        }
        return true;
    }
};