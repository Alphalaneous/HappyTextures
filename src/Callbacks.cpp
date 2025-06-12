#include "Callbacks.hpp"
#include "Macros.hpp"
#include "UIModding.hpp"

Callbacks* Callbacks::instance = nullptr;

void Callbacks::generateAll() {
    if (m_generated) return;
    UIModding::get()->skipCheck = true;
    generateMenuLayerCallbacks();
    generateCreatorLayerCallbacks();
    generateGarageCallbacks();
    UIModding::get()->skipCheck = false;
    m_generated = true;
}

CCMenuItemSpriteExtra* Callbacks::getDummyButton() {
    if (!m_dummyButton) {
        m_dummyButton = CCMenuItemSpriteExtra::create(CCSprite::create(), nullptr, nullptr);
        m_dummyButton->setUserObject("dummy"_spr, CCBool::create(true));
    }
    return m_dummyButton;
}

struct MenuLayerCallbacks : public CCNode {

    void onPlay(CCObject* obj) {
        CCDirector::get()->pushScene(CCTransitionFade::create(0.5, LevelSelectLayer::scene(0)));
    }

    void onCreator(CCObject* obj) {
        CCDirector::get()->pushScene(CCTransitionFade::create(0.5, CreatorLayer::scene()));    
    }

    void onGarage(CCObject* obj) {
        CCDirector::get()->pushScene(CCTransitionFade::create(0.5, GJGarageLayer::scene()));
    }

};

void Callbacks::generateMenuLayerCallbacks() {
    CREATE_NORMAL(MenuLayer);
    m_callbacks["MenuLayer"]["onPlay"] = std::pair<CCNode*, cocos2d::SEL_MenuHandler>(self, menu_selector(MenuLayerCallbacks::onPlay));
    m_callbacks["MenuLayer"]["onGarage"] = std::pair<CCNode*, cocos2d::SEL_MenuHandler>(self, menu_selector(MenuLayerCallbacks::onGarage));
    m_callbacks["MenuLayer"]["onCreator"] = std::pair<CCNode*, cocos2d::SEL_MenuHandler>(self, menu_selector(MenuLayerCallbacks::onCreator));
    REGISTER_CALLBACK(MenuLayer, onAchievements);
    REGISTER_CALLBACK(MenuLayer, onDaily);
    REGISTER_CALLBACK(MenuLayer, onMoreGames);
    REGISTER_CALLBACK(MenuLayer, onMyProfile);
    REGISTER_CALLBACK(MenuLayer, onOptions);
    REGISTER_CALLBACK(MenuLayer, onStats);
}

void Callbacks::generateCreatorLayerCallbacks() {
    CREATE_NORMAL(CreatorLayer);
    REGISTER_CALLBACK(CreatorLayer, onAdventureMap);
    REGISTER_CALLBACK(CreatorLayer, onChallenge);
    REGISTER_CALLBACK(CreatorLayer, onDailyLevel);
    REGISTER_CALLBACK(CreatorLayer, onEventLevel);
    REGISTER_CALLBACK(CreatorLayer, onFeaturedLevels);
    REGISTER_CALLBACK(CreatorLayer, onGauntlets);
    REGISTER_CALLBACK(CreatorLayer, onLeaderboards);
    REGISTER_CALLBACK(CreatorLayer, onMapPacks);
    REGISTER_CALLBACK(CreatorLayer, onMyLevels);
    REGISTER_CALLBACK(CreatorLayer, onOnlineLevels);
    REGISTER_CALLBACK(CreatorLayer, onPaths);
    REGISTER_CALLBACK(CreatorLayer, onSavedLevels);
    REGISTER_CALLBACK(CreatorLayer, onSecretVault);
    REGISTER_CALLBACK(CreatorLayer, onTopLists);
    REGISTER_CALLBACK(CreatorLayer, onTreasureRoom);
    REGISTER_CALLBACK(CreatorLayer, onWeeklyLevel);
}

void Callbacks::generateGarageCallbacks() {
    CREATE_NORMAL(GJGarageLayer);
    REGISTER_CALLBACK(GJGarageLayer, onShards);
    REGISTER_CALLBACK(GJGarageLayer, onShop);
}
