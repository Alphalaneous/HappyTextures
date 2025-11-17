#pragma once

#include <Geode/Geode.hpp>
#include "Macros.hpp"
#include "StringUtils.hpp"

using namespace geode::prelude;

class Callbacks {

protected:
    static Callbacks* instance;
    bool m_generated = false;
public:
    std::unordered_map<std::string, std::unordered_map<std::string, std::pair<CCNode*, cocos2d::SEL_MenuHandler>, StringHash, StringEq>, StringHash, StringEq> m_callbacks;
    std::unordered_map<std::string, Ref<CCNode>, StringHash, StringEq> m_layers;
    Ref<CCMenuItemSpriteExtra> m_dummyButton;

    void generateAll();
    void generateMenuLayerCallbacks();
    void generateCreatorLayerCallbacks();
    void generateGarageCallbacks();

    CCMenuItemSpriteExtra* getDummyButton();

    static Callbacks* get() {

        if (!instance) {
            instance = new Callbacks();
        };
        return instance;
    }
};