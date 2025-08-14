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

class $nodeModify(MyCCMenu, cocos2d::CCMenu) {
    void modify() {
        if (UIModding::get()->doModify) {
            schedule(schedule_selector(MyCCMenu::checkMouse));
        }
    }

    static CCNode* getSceneChildContainingNode(CCNode* node) {
        if (!node) return nullptr;
        auto current = node;
        while (current && current->getParent() != CCScene::get()) {
            current = current->getParent();
        }
        return current;
    }

    bool isLastAlert(CCNode* node) {
        bool shouldCheck = false;
        bool lastAlert = false;

        if (auto child = getSceneChildContainingNode(node)) {
            for (auto c : CCArrayExt<CCNode*>(child->getChildren())) {
                if (!AlphaUtils::Cocos::hasNode(node, c)) {
                    shouldCheck = true;
                }
                if (shouldCheck) {
                    if (typeinfo_cast<FLAlertLayer*>(c) || typeinfo_cast<CCBlockLayer*>(c)) {
                        if (AlphaUtils::Cocos::hasNode(node, c)) continue;
                        lastAlert = true;
                    }
                }
            }
        }
        return lastAlert;
    }


    bool isHoverable(CCNode* node, CCPoint point) {
        if (!CCScene::get() || !node || isLastAlert(node)) return false;

        auto sceneChild = getSceneChildContainingNode(node);
        if (!sceneChild) return false;

        if (node->getChildrenCount() == 1) {
            if (typeinfo_cast<ButtonSprite*>(node->getChildren()->objectAtIndex(0))) {
                return false;
            }
        }

        for (auto child : CCArrayExt<CCNode*>(CCScene::get()->getChildren())) {
            if (child->getZOrder() <= sceneChild->getZOrder()) continue;
            if (child->boundingBox().containsPoint(point) && nodeIsVisible(child)) {
                return false;
            }
        }
        return true;
    }

    void checkMouse(float) {
        if (!nodeIsVisible(this)) return;

#ifdef GEODE_IS_DESKTOP
        auto mousePos = getMousePos();
        auto local = convertToNodeSpace(mousePos);
        for (auto child : CCArrayExt<CCNode*>(getChildren())) {
            auto realItem = typeinfo_cast<CCMenuItemSpriteExtra*>(child);
            if (!realItem) continue;
            if (EventCCMenuItemSpriteExtra* button = static_cast<EventCCMenuItemSpriteExtra*>(realItem)) {
                auto worldPos = button->convertToWorldSpaceAR(CCPointZero);
                bool isValid = nodeIsVisible(button) && button->boundingBox().containsPoint(local) && isHoverable(button, worldPos);
                button->checkTouch(!isValid);
            }
        }
#else
        auto self = reinterpret_cast<CCMenu*>(this);
        if (auto item = self->m_pSelectedItem) {
            auto realItem = typeinfo_cast<CCMenuItemSpriteExtra*>(child);
            if (!realItem) continue;
            if (EventCCMenuItemSpriteExtra* button = static_cast<EventCCMenuItemSpriteExtra*>(realItem)) {
                button->checkTouch(!button->isSelected());
            }
        }
#endif
    }
};