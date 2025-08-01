#pragma once

#include <Geode/Geode.hpp>
#include "Macros.hpp"

using namespace geode::prelude;

class Callbacks {

protected:
    static Callbacks* instance;
    bool m_generated = false;
public:
    std::map<std::string, std::map<std::string, std::pair<CCNode*, cocos2d::SEL_MenuHandler>>> m_callbacks;
    std::map<std::string, Ref<CCNode>> m_layers;
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