#pragma once

#include <Geode/Geode.hpp>
#include "CCMenuItemSpriteExtra.hpp"
#include "../UIModding.hpp"
#include "../Utils.hpp"
#include "../Macros.hpp"
#include <alphalaneous.alphas_geode_utils/include/NodeModding.h>

using namespace geode::prelude;

class EventCCMenuItem;
class KeybindsLayer : CCLayer{};
class EventsPush : CCNode {};

class $nodeModify(EventCCMenu, CCMenu) {

    struct Fields {
        bool hasLayerOnTop = true;
        bool canExit = false;
        int lastLayerCount = 0;
        CCScene* currentScene = nullptr;
    };

    void modify() {
        if (UIModding::get()->doModify) {
            schedule(schedule_selector(EventCCMenu::check), 1/15.f);
        }
    }

    void checkTouch(CCNode* node, bool hasLayerOnTop) {

        for (CCNode* nodeA : CCArrayExt<CCNode*>(node->getChildren())) {
            if (nodeA && nodeIsVisible(nodeA)) {
                if (EventCCMenuItemSpriteExtra* button = static_cast<EventCCMenuItemSpriteExtra*>(nodeA)) {
                    button->checkTouch(hasLayerOnTop);
                }
                if (CCMenuItemToggler* toggler = typeinfo_cast<CCMenuItemToggler*>(nodeA)) {
                    checkTouch(nodeA, hasLayerOnTop);
                }
            }
        }
    }

    void check(float dt) {
        
        if (!nodeIsVisible(this)) return;

        CCScene* currentScene = CCDirector::get()->getRunningScene();
        int layerCount = currentScene->getChildrenCount();

        if (layerCount != m_fields->lastLayerCount || currentScene != m_fields->currentScene) {

            bool hasLayerOnTop = true;
            bool gotNode = false;

            for (CCNode* node : CCArrayExt<CCNode*>(currentScene->getChildren())) {
                if (!gotNode && Utils::hasNode(this, node)) {
                    if (typeinfo_cast<KeybindsLayer*>(node)) return;
                    gotNode = true;
                    
                    hasLayerOnTop = false;
                    continue;
                }
                if (gotNode && node->getContentSize() != CCSize{0,0} && node->isVisible()) {
                    if (!typeinfo_cast<geode::Notification*>(node) && !typeinfo_cast<EventsPush*>(node) && node->getID() != "itzkiba.better_progression/tier-popup" && node->getID() != "dankmeme.globed2/notification-panel") {
                        hasLayerOnTop = true; 
                    }
                    break;
                }
            }
           
            m_fields->hasLayerOnTop = hasLayerOnTop;
            m_fields->lastLayerCount = layerCount;
            m_fields->currentScene = currentScene;
        }

        if (!m_fields->hasLayerOnTop) {
            checkTouch(this, m_fields->hasLayerOnTop);
            m_fields->canExit = true;
        }
        else if (m_fields->canExit) {
            checkTouch(this, m_fields->hasLayerOnTop);
            m_fields->canExit = false;
        }
    }
};