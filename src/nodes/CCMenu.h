#pragma once

#ifndef MYCCMENU_H
#define MYCCMENU_H

#include <Geode/Geode.hpp>
#include <Geode/modify/CCMenu.hpp>
#include "CCMenuItemSpriteExtra.h"
#include "../UIModding.h"

using namespace geode::prelude;

class EventCCMenuItemSpriteExtra;

class $modify(EventCCMenu, CCMenu){

    struct Fields {
        bool hasLayerOnTop = true;
        bool canExit = false;
        int lastLayerCount = 0;
    };

    bool initWithArray(CCArray* array){
        if(!CCMenu::initWithArray(array)) return false;
        if(UIModding::get()->doModify){
    #ifdef GEODE_IS_WINDOWS
            schedule(schedule_selector(EventCCMenu::check), 1/15);
    #endif
        }
        return true;
    }

    CCNode* findNodeRecursive(CCNode* node, CCNode* child) {

        if(node == child){
            return child;
        }

        for (auto childa : CCArrayExt<CCNode*>(child->getChildren())) {
            if ((childa = findNodeRecursive(node, childa))) {
                return childa;
            }
        }
        return nullptr;
    }

    void recursiveCheck(CCNode* node, bool hasLayerOnTop){

        for(CCNode* node : CCArrayExt<CCNode*>(node->getChildren())){
            if(EventCCMenuItemSpriteExtra* button = static_cast<EventCCMenuItemSpriteExtra*>(node)){
                button->checkTouch(hasLayerOnTop);
            }
            recursiveCheck(node, hasLayerOnTop);
        }
    }

    void check(float dt){

        if(LevelEditorLayer::get()) return;

        CCScene* currentScene = CCDirector::get()->getRunningScene();
        int layerCount = currentScene->getChildrenCount();

        if(layerCount != m_fields->lastLayerCount){
            CCNode* buttonLayer;

            for(CCNode* node : CCArrayExt<CCNode*>(currentScene->getChildren())){
                if(findNodeRecursive(this, node)){
                    buttonLayer = node;
                }
            }

            bool hasLayerOnTop = true;
            bool doCheck = false;
            for(CCNode* node : CCArrayExt<CCNode*>(currentScene->getChildren())){
                if(node == buttonLayer) {
                    doCheck = true;
                    hasLayerOnTop = false;
                    continue;
                }
                if(doCheck){
                    if(node->getContentSize() != CCSize{0,0} && node->isVisible()){
                        hasLayerOnTop = true;
                        break;
                    }
                }
            }
            m_fields->hasLayerOnTop = hasLayerOnTop;
            m_fields->lastLayerCount = layerCount;
        }

        if(!m_fields->hasLayerOnTop){
            recursiveCheck(this, m_fields->hasLayerOnTop);
            m_fields->canExit = true;
        }
        else if(m_fields->canExit){
            recursiveCheck(this, m_fields->hasLayerOnTop);
            m_fields->canExit = false;
        }
    }
};

#endif