#pragma once

#ifndef MYCCSCENE_H
#define MYCCSCENE_H

#include <Geode/Geode.hpp>
#include <Geode/modify/CCScene.hpp>
#include "../UIModding.h"

using namespace geode::prelude;

class $modify(MyCCScene, CCScene){

    static void onModify(auto& self) {
        (void) self.setHookPriority("CCScene::create", INT_MIN);
    }

    struct Fields {
        int m_currentCount = 0;
        bool m_isMenuLayer = false;
    };

    static CCScene* create(){
        auto ret = CCScene::create();
        if(UIModding::get()->doModify){
            ret->schedule(schedule_selector(MyCCScene::checkForUpdates));
        }
        return ret;
    }

    void checkForUpdates(float dt){
        if(this->getChildrenCount() != m_fields->m_currentCount && (this->getChildrenCount() != 1 || m_fields->m_currentCount == 0)){
            int idx = 0;
           
            for(CCNode* node : CCArrayExt<CCNode*>(this->getChildren())){
                idx++;
                if(node->getID() == "MenuLayer") continue;
                if(idx > m_fields->m_currentCount){
                    //UIModding::get()->doUICheck(node);
                }
            }
        }
        m_fields->m_currentCount = this->getChildrenCount();
    }
};

#endif