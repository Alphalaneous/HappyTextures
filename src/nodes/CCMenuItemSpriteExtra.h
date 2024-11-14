#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCMenuItemSpriteExtra.hpp>
#include "CCScene.h"
#include "../Macros.h"

using namespace geode::prelude;

class $modify(EventCCMenuItemSpriteExtra, CCMenuItemSpriteExtra) {

    static void onModify(auto& self) {
        (void) self.setHookPriority("CCMenuItemSpriteExtra::selected", INT_MAX-1);
        (void) self.setHookPriority("CCMenuItemSpriteExtra::unselected", INT_MAX-1);
        (void) self.setHookPriority("CCMenuItemSpriteExtra::activate", INT_MAX-1);
    }

    struct Fields {
        matjson::Value onClick;
        matjson::Value onRelease;
        matjson::Value onActivate;
        matjson::Value onHover;
        matjson::Value onExit;
        bool overrideOnClick = false;
        bool overrideOnRelease = false;
        bool overrideOnActivate = false;
        bool isHovering = false;
        bool validHover = false;
        bool hasHover = false;
        bool hasExit = false;
        ccColor3B originalColor;
        unsigned char originalOpacity;
    };
    void setOnClick(matjson::Value onClick) {
        m_fields->onClick = onClick;
        if (onClick.contains("override")) {
            matjson::Value overrideVal = onClick["override"];
            if (overrideVal.isBool()) {
                m_fields->overrideOnClick = overrideVal.asBool().unwrapOr(false);
            }
        }
    }
    void setOnRelease(matjson::Value onRelease) {
        m_fields->onRelease = onRelease;
        if (onRelease.contains("override")) {
            matjson::Value overrideVal = onRelease["override"];
            if (overrideVal.isBool()) {
                m_fields->overrideOnRelease = overrideVal.asBool().unwrapOr(false);
            }
        }
    }
    void setOnActivate(matjson::Value onActivate) {
        m_fields->onActivate = onActivate;
        if (onActivate.contains("override")) {
            matjson::Value overrideVal = onActivate["override"];
            if (overrideVal.isBool()) {
                m_fields->overrideOnActivate = overrideVal.asBool().unwrapOr(false);
            }
        }
    }
    void setOnHover(matjson::Value onHover) {
        m_fields->hasHover = true;
        m_fields->onHover = onHover;
    }
    void setOnExit(matjson::Value onExit) {
        m_fields->hasExit = true;
        m_fields->onExit = onExit;
    }

    void runOnHover() {
        UIModding::get()->handleModifications(this, m_fields->onHover);
    }

    void runOnExit() {
        UIModding::get()->handleModifications(this, m_fields->onExit);
    }

    void selected() {
        
        SAFE_RUN(
            if (!m_fields->overrideOnClick) {
                CCMenuItemSpriteExtra::selected();
            }
            UIModding::get()->handleModifications(this, m_fields->onClick);
        )
    }

    void unselected() {

        SAFE_RUN(
            if (!m_fields->overrideOnRelease) {
                CCMenuItemSpriteExtra::unselected();
            }
            UIModding::get()->handleModifications(this, m_fields->onRelease);
        )
    }

    void activate() {

        SAFE_RUN(
            if (!m_fields->overrideOnActivate) {
                CCMenuItemSpriteExtra::activate();
            }
            UIModding::get()->handleModifications(this, m_fields->onActivate);
        )
    }

    void checkTouch(bool hasLayerOnTop) {

        if ((m_fields->hasHover || m_fields->hasExit) && nodeIsVisible(this)) {

            CCPoint point = getMousePos();

            CCMenu* parentMenu = static_cast<CCMenu*>(getParent());
        
            CCPoint local = convertToNodeSpace(point);
            CCRect r = rect();
            r.origin = CCPointZero;

            CCSize screenSize = CCDirector::get()->getWinSize();

            CCRect screenRect = CCRect{0, 0, screenSize.width, screenSize.height};

            bool containsPoint = r.containsPoint(local);
        
            if (!hasLayerOnTop) {
                if (containsPoint && !m_fields->isHovering) {
                    m_fields->isHovering = true;

                    m_fields->originalColor = getColor();
                    m_fields->originalOpacity = getOpacity();

                    if (ButtonSprite* node = this->getChildByType<ButtonSprite>(0)) {

                        m_fields->originalColor = node->getColor();
                        m_fields->originalOpacity = node->getOpacity();

                        if (node->getColor() == ccColor3B{255,255,255}) {
                            if (CCSprite* node1 = node->getChildByType<CCSprite>(0)) {
                                m_fields->originalColor = node1->getColor();
                                m_fields->originalOpacity = node1->getOpacity();
                            }
                        }
                    }
                    runOnHover();
                }
                if (!containsPoint && m_fields->isHovering) {
                    m_fields->isHovering = false;
                    runOnExit();
                }
            }
            else if (m_fields->isHovering) {
                m_fields->isHovering = false;
                runOnExit();
            }
        }
    }
};