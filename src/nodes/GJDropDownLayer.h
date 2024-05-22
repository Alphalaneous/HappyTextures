#pragma once

#ifndef MYGJDROPDOWNLAYER_H
#define MYGJDROPDOWNLAYER_H

#include <Geode/Geode.hpp>
#include <Geode/modify/GJDropDownLayer.hpp>

using namespace geode::prelude;

class $modify(MyGJDropDownLayer, GJDropDownLayer){

    static void onModify(auto& self) {
        (void) self.setHookPriority("GJDropDownLayer::showLayer", INT_MIN);
    }

    void showLayer(bool p0){
        GJDropDownLayer::showLayer(p0);
        #ifndef GEODE_IS_MACOS
        if(UIModding::get()->doModify){
            removeFromParent();
            CCScene* currentScene = CCDirector::get()->getRunningScene();
            currentScene->addChild(this);

            CCMoveTo* moveTo = CCMoveTo::create(0.5, m_endPosition);
            CCEaseInOut* easeInOut = CCEaseInOut::create(moveTo, 2.0);
            CCSequence* sequence = CCSequence::create(easeInOut, 0);

            m_mainLayer->runAction(sequence);

            CCFadeTo* fadeTo = CCFadeTo::create(0.5, 125);
            runAction(fadeTo);

            if(UIModding::get()->doModify){
                UIModding::get()->doUICheck(this);
            }
        }
        #endif
    }
};

#endif