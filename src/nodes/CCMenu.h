#pragma once

#ifndef MYCCMENU_H
#define MYCCMENU_H

#ifdef GEODE_IS_WINDOWS

#include <Geode/Geode.hpp>
#include <Geode/modify/CCMenu.hpp>
#include "CCMenuItemSpriteExtra.h"
#include "../UIModding.h"

using namespace geode::prelude;

class EventCCMenuItem;
class KeybindsLayer {

};
class EventsPush {

};

class $modify(EventCCMenu, CCMenu){

    static void onModify(auto& self) {
        (void) self.setHookPriority("CCMenu::initWithArray", INT_MAX);
    }

    struct Fields {
        bool hasLayerOnTop = true;
        bool canExit = false;
        int lastLayerCount = 0;
        CCScene* currentScene = nullptr;
    };

    bool initWithArray(CCArray* array){
        if(!CCMenu::initWithArray(array)) return false;
        if(UIModding::get()->doModify){
            schedule(schedule_selector(EventCCMenu::check), 1/15);
        }
        return true;
    }

    bool hasNode(CCNode* child, CCNode* node) {
        CCNode* parent = child;
        while(true){
            if(parent){
                if(parent == node){
                    return true;
                }
                parent = parent->getParent();
            }
            else break;
        }
        return false;
    }
   
    void checkTouch(CCNode* node, bool hasLayerOnTop){

        for(CCNode* nodeA : CCArrayExt<CCNode*>(node->getChildren())){
            if(nodeIsVisible(nodeA)){
                if(EventCCMenuItemSpriteExtra* button = static_cast<EventCCMenuItemSpriteExtra*>(nodeA)){
                    button->checkTouch(hasLayerOnTop);
                }
                if(CCMenuItemToggler* toggler = dynamic_cast<CCMenuItemToggler*>(nodeA)){
                    checkTouch(nodeA, hasLayerOnTop);
                }
            }
        }
    }

    void check(float dt){
        
        if(!nodeIsVisible(this)) return;

        CCScene* currentScene = CCDirector::get()->getRunningScene();
        int layerCount = currentScene->getChildrenCount();

        if(layerCount != m_fields->lastLayerCount || currentScene != m_fields->currentScene){

            bool hasLayerOnTop = true;
            bool gotNode = false;

            for(CCNode* node : CCArrayExt<CCNode*>(currentScene->getChildren())){
                if(!gotNode && hasNode(this, node)){
                    if(typeinfo_cast<KeybindsLayer*>(node)) return;
                    gotNode = true;
                    
                    hasLayerOnTop = false;
                    continue;
                }
                if(gotNode && node->getContentSize() != CCSize{0,0} && node->isVisible()){
                    log::info("{}", node->getID());
                    if(!typeinfo_cast<geode::Notification*>(node) && !typeinfo_cast<EventsPush*>(node) && node->getID() != "itzkiba.better_progression/tier-popup"){
                        hasLayerOnTop = true; 
                    }
                    break;
                }
            }
           
            m_fields->hasLayerOnTop = hasLayerOnTop;
            m_fields->lastLayerCount = layerCount;
            m_fields->currentScene = currentScene;
        }

        if(!m_fields->hasLayerOnTop){
            checkTouch(this, m_fields->hasLayerOnTop);
            m_fields->canExit = true;
        }
        else if(m_fields->canExit){
            checkTouch(this, m_fields->hasLayerOnTop);
            m_fields->canExit = false;
        }
    }
};

#endif

#endif