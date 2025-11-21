#pragma once

#include <Geode/Geode.hpp>
#include "CCMenuItemSpriteExtra.hpp"
#include "../UIModding.hpp"
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
            if (!child) return false;
            for (auto c : CCArrayExt<CCNode*>(child->getChildren())) {
                if (!c) continue;
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


    bool isHoverable(CCNode* node) {
        if (!CCScene::get() || !node || isLastAlert(node)) return false;
        auto worldPos = node->convertToWorldSpaceAR(CCPointZero);

        auto sceneChild = getSceneChildContainingNode(node);
        if (!sceneChild) return false;

        for (auto child : CCScene::get()->getChildrenExt()) {
            if (child->getZOrder() <= sceneChild->getZOrder()) continue;
            if (child->boundingBox().containsPoint(worldPos) && nodeIsVisible(child)) {
                return false;
            }
        }
        return true;
    }

    void checkMouse(float) {
        if (!nodeIsVisible(this)) return;

        auto mousePos = getMousePos();
        for (auto child : CCArrayExt<CCNode*>(getChildren())) {
            if (!nodeIsVisible(child)) continue;
            auto realItem = typeinfo_cast<CCMenuItemSpriteExtra*>(child);
            if (!realItem) continue;

            if (EventCCMenuItemSpriteExtra* button = static_cast<EventCCMenuItemSpriteExtra*>(realItem)) {
                auto worldPos = button->convertToWorldSpace(CCPointZero);

                auto nodeMouse = convertToNodeSpace(mousePos);

                bool isValid = button->boundingBox().containsPoint(nodeMouse) && isHoverable(button);
                button->checkTouch(!isValid);
            }
        }
    }
};

