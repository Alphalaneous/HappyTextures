#pragma once

#ifndef MYCCMENUITEMSPRITEXTRA_H
#define MYCCMENUITEMSPRITEXTRA_H

#include <Geode/Geode.hpp>
#include <Geode/modify/CCMenuItemSpriteExtra.hpp>
#include "CCScene.h"

using namespace geode::prelude;

struct EventCCMenuItemSpriteExtra : Modify<EventCCMenuItemSpriteExtra, CCMenuItemSpriteExtra> {

    static void onModify(auto& self) {\
        (void) self.setHookPriority("CCMenuItemSpriteExtra::create", INT_MIN);\
    }

    struct Fields {
        matjson::Object onClick;
        matjson::Object onRelease;
        matjson::Object onActivate;
        matjson::Object onHover;
        matjson::Object onExit;
        bool overrideOnClick = false;
        bool overrideOnRelease = false;
        bool overrideOnActivate = false;
        bool isHovering = false;
        bool validHover = false;
        bool hasLayerOnTop = true;
        int lastLayerCount = 0;
        bool hasHover = false;
        bool hasExit = false;
        CCNode* runner = CCNode::create();
    };
    void setOnClick(matjson::Object onClick);
    void setOnRelease(matjson::Object onRelease);
    void setOnActivate(matjson::Object onActivate);
    void setOnHover(matjson::Object onHover);
    void setOnExit(matjson::Object onExit);
    void runOnClick();
    void runOnRelease();
    void runOnActivate();
    void runOnHover();
    void runOnExit();
    void selected();
    void unselected();
    void activate();
    void checkTouch(float dt);
    static CCMenuItemSpriteExtra* create(cocos2d::CCNode* p0, cocos2d::CCNode* p1, cocos2d::CCObject* p2, cocos2d::SEL_MenuHandler p3);
    static CCMenuItemSpriteExtra* create(cocos2d::CCNode* p0, cocos2d::CCObject* p1, cocos2d::SEL_MenuHandler p2);
};

#endif