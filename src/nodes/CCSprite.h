#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCSprite.hpp>
#include <Geode/modify/CCSpriteWithHue.hpp>
#include "../Macros.h"

class $modify(MyCCSpriteWithHue, CCSpriteWithHue) {

    static void onModify(auto& self) {
        (void) self.setHookPriority("cocos2d::CCSpriteWithHue::initWithSpriteFrame", Priority::VeryEarly);
    }

    bool initWithSpriteFrame(cocos2d::CCSpriteFrame* p0) {
        std::string frameName = Utils::getSpriteName(p0);

        if (Utils::spriteExistsInPacks(frameName)) {
            CCSprite* spr = CCSprite::create(frameName.c_str());
            auto spriteFrame = CCSpriteFrame::createWithTexture(spr->getTexture(), spr->getTextureRect());
            CCSpriteFrameCache::get()->addSpriteFrame(spriteFrame, frameName.c_str());
            return CCSpriteWithHue::initWithSpriteFrame(spriteFrame);
        }

        return CCSpriteWithHue::initWithSpriteFrame(p0);
    }
};

class $modify(MyCCSprite, CCSprite) {

    static void onModify(auto& self) {
        (void) self.setHookPriority("cocos2d::CCSprite::initWithSpriteFrameName", Priority::VeryEarly);
        (void) self.setHookPriority("cocos2d::CCSprite::createWithSpriteFrameName", Priority::VeryEarly);
        (void) self.setHookPriority("cocos2d::CCSprite::setDisplayFrame", Priority::VeryEarly);
    }
    
    void setDisplayFrame(CCSpriteFrame *pNewFrame) {
        std::string frameName = Utils::getSpriteName(pNewFrame);
        if (Utils::spriteExistsInPacks(frameName)) {
            CCSprite* spr = CCSprite::create(frameName.c_str());
            auto spriteFrame = CCSpriteFrame::createWithTexture(spr->getTexture(), spr->getTextureRect());
            CCSpriteFrameCache::get()->addSpriteFrame(spriteFrame, frameName.c_str());
            return CCSprite::setDisplayFrame(spriteFrame);
        }
        CCSprite::setDisplayFrame(pNewFrame);
    }

    bool initWithSpriteFrameName(const char *pszSpriteFrameName) {
        std::string copy = pszSpriteFrameName;
        
        if (Utils::spriteExistsInPacks(copy)) {
            CCSprite* spr = CCSprite::create(copy.c_str());
            auto spriteFrame = CCSpriteFrame::createWithTexture(spr->getTexture(), spr->getTextureRect());
            CCSpriteFrameCache::get()->addSpriteFrame(spriteFrame, copy.c_str());
        }

        return CCSprite::initWithSpriteFrameName(copy.c_str());
    }
    
    static CCSprite* createWithSpriteFrameName(const char *pszSpriteFrameName) {
        std::string copy = pszSpriteFrameName;

        if (Utils::spriteExistsInPacks(copy)) {

            CCSprite* spr = CCSprite::create(copy.c_str());
            auto spriteFrame = CCSpriteFrame::createWithTexture(spr->getTexture(), spr->getTextureRect());
            CCSpriteFrameCache::get()->addSpriteFrame(spriteFrame, copy.c_str());
        }

        return CCSprite::createWithSpriteFrameName(copy.c_str());
    }
};
