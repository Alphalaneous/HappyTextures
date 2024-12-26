#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCSprite.hpp>
#include "../Macros.h"

class $modify(MyCCSprite, CCSprite) {

    static void onModify(auto& self) {
        (void) self.setHookPriority("cocos2d::CCSprite::createWithSpriteFrameName", Priority::VeryEarly);
    }

    static CCSprite* createWithSpriteFrameName(const char *pszSpriteFrameName) {
        if (Utils::spriteExistsInPacks(pszSpriteFrameName)) {
            auto ret = CCSprite::create(pszSpriteFrameName); 
            if (ret && !ret->getUserObject("geode.texture-loader/fallback")) {
                ret->setUserObject("single"_spr, CCBool::create(true));
                return ret;
            }
        }

        return CCSprite::createWithSpriteFrameName(pszSpriteFrameName);
    }
};

CCNode* myGetChildBySpriteFrameName(CCNode* parent, const char* name) {

    for (auto child : CCArrayExt<CCNode*>(parent->getChildren())) {
        if (static_cast<CCBool*>(child->getUserObject("single"_spr))) {
            if (geode::cocos::isSpriteName(static_cast<CCNode*>(child), name)) {
                return child;
            }
        }
    }

    return geode::cocos::getChildBySpriteFrameName(parent, name);
}

$execute {
    (void) Mod::get()->hook(
        reinterpret_cast<void*>(
            geode::addresser::getNonVirtual(
                geode::modifier::Resolve<CCNode*, const char*>::func(&geode::cocos::getChildBySpriteFrameName)
            )
        ),
        &myGetChildBySpriteFrameName,
        "geode::cocos::getChildBySpriteFrameName",
        tulip::hook::TulipConvention::Cdecl
    );
}