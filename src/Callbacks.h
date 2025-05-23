#pragma once

#include <Geode/Geode.hpp>
#include "Macros.h"

using namespace geode::prelude;

template <typename T>
struct FakeNodeLayer final {
    char m_alloc[sizeof(T)];
    T* operator->() {
        return get();
    }
    T* get() {
        return reinterpret_cast<T*>(&m_alloc);
    }
};

class Callbacks {

protected:
    static Callbacks* instance;
    bool m_generated = false;
public:
    bool m_ignoreUICheck = false;
    std::map<std::string, std::map<std::string, std::pair<CCNode*, cocos2d::SEL_MenuHandler>>> m_callbacks;
    std::map<std::string, Ref<CCNode>> m_layers;
    Ref<CCMenuItemSpriteExtra> m_dummyButton;

    template <typename T>
    FakeNodeLayer<T> createUBDummyLayer();

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