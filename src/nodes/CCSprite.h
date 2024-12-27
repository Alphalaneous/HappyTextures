#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCSprite.hpp>
#include "../Macros.h"

class $modify(MyCCSprite, CCSprite) {

    static void onModify(auto& self) {
        (void) self.setHookPriority("cocos2d::CCSprite::initWithSpriteFrameName", Priority::VeryEarly);
    }

    bool initWithSpriteFrameName(const char *pszSpriteFrameName) {
        if (Utils::spriteExistsInPacks(pszSpriteFrameName)) {

            CCSprite* spr = CCSprite::create(pszSpriteFrameName);
            auto spriteFrame = CCSpriteFrame::createWithTexture(spr->getTexture(), spr->getTextureRect());
            CCSpriteFrameCache::get()->addSpriteFrame(spriteFrame, pszSpriteFrameName);
        }

        return CCSprite::initWithSpriteFrameName(pszSpriteFrameName);
    }
};
