#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCMenuItemSpriteExtra.hpp>
#include "../Macros.hpp"
#include "../UIModding.hpp"

using namespace geode::prelude;

class $modify(EventCCMenuItemSpriteExtra, CCMenuItemSpriteExtra) {

    struct Fields {
        std::vector<matjson::Value> onClick = {};
        std::vector<matjson::Value> onRelease = {};
        std::vector<matjson::Value> onActivate = {};
        std::vector<matjson::Value> onHover = {};
        std::vector<matjson::Value> onExit = {};
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
        auto fields = m_fields.self();

        fields->onClick.push_back(onClick);
        if (onClick.contains("override")) {
            matjson::Value overrideVal = onClick["override"];
            if (overrideVal.isBool()) {
                fields->overrideOnClick = overrideVal.asBool().unwrapOr(false);
            }
        }
    }

    void setOnRelease(matjson::Value onRelease) {
        auto fields = m_fields.self();

        fields->onRelease.push_back(onRelease);
        if (onRelease.contains("override")) {
            matjson::Value overrideVal = onRelease["override"];
            if (overrideVal.isBool()) {
                fields->overrideOnRelease = overrideVal.asBool().unwrapOr(false);
            }
        }
    }
    void setOnActivate(matjson::Value onActivate) {
        auto fields = m_fields.self();

        fields->onActivate.push_back(onActivate);
        if (onActivate.contains("override")) {
            matjson::Value overrideVal = onActivate["override"];
            if (overrideVal.isBool()) {
                fields->overrideOnActivate = overrideVal.asBool().unwrapOr(false);
            }
        }
    }

    void setOnHover(matjson::Value onHover) {
        auto fields = m_fields.self();

        fields->hasHover = true;
        fields->onHover.push_back(onHover);
    }
    
    void setOnExit(matjson::Value onExit) {
        auto fields = m_fields.self();

        fields->hasExit = true;
        fields->onExit.push_back(onExit);
    }

    void modifyForEach(const std::vector<matjson::Value>& values) {
        for (const matjson::Value& value : values) {
            UIModding::get()->handleModifications(this, value);
        }
    }

    void runOnHover() {
        modifyForEach(m_fields->onHover);
    }

    void runOnExit() {
        modifyForEach(m_fields->onExit);
    }

    void selected() {
        modifyForEach(m_fields->onClick);
        if (!m_fields->overrideOnClick) {
            CCMenuItemSpriteExtra::selected();
        }
    }

    void unselected() {
        modifyForEach(m_fields->onRelease);
        if (!m_fields->overrideOnRelease) {
            CCMenuItemSpriteExtra::unselected();
        }
    }

    void activate() {
        modifyForEach(m_fields->onActivate);
        if (!m_fields->overrideOnActivate) {
            CCMenuItemSpriteExtra::activate();
        }
    }

    void checkTouch(bool shouldExit) {
        auto fields = m_fields.self();
        if (fields->hasHover || fields->hasExit) {
            if (!shouldExit && !fields->isHovering) {
                fields->isHovering = true;

                fields->originalColor = getColor();
                fields->originalOpacity = getOpacity();

                if (ButtonSprite* node = this->getChildByType<ButtonSprite>(0)) {

                    fields->originalColor = node->getColor();
                    fields->originalOpacity = node->getOpacity();

                    if (node->getColor() == ccColor3B{255,255,255}) {
                        if (CCSprite* node1 = node->getChildByType<CCSprite>(0)) {
                            fields->originalColor = node1->getColor();
                            fields->originalOpacity = node1->getOpacity();
                        }
                    }
                }
                runOnHover();
            }
            if (shouldExit && fields->isHovering) {
                fields->isHovering = false;
                runOnExit();
            }
        }
    }
};