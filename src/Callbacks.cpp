#include "Callbacks.h"
#include "Macros.h"

Callbacks* Callbacks::instance = nullptr;

//do cursed UB stuff, currently unused but may be useful in the future

template <typename T>
FakeNodeLayer<T> Callbacks::createUBDummyLayer() {
    return FakeNodeLayer<T>();
}

void Callbacks::generateAll() {
    if (m_generated) return;
    m_ignoreUICheck = true;
    generateMenuLayerCallbacks();
    generateCreatorLayerCallbacks();
    generateGarageCallbacks();
    m_ignoreUICheck = false;
    m_generated = true;
}

CCMenuItemSpriteExtra* Callbacks::getDummyButton() {
    if (!m_dummyButton) {
        m_dummyButton = CCMenuItemSpriteExtra::create(CCSprite::create(), nullptr, nullptr);
        m_dummyButton->setUserObject("dummy"_spr, CCBool::create(true));
    }
    return m_dummyButton;
}

void Callbacks::generateMenuLayerCallbacks() {
    CREATE_NORMAL(MenuLayer);
    REGISTER_CALLBACK(MenuLayer, onPlay);
    REGISTER_CALLBACK(MenuLayer, onAchievements);
    REGISTER_CALLBACK(MenuLayer, onCreator);
    REGISTER_CALLBACK(MenuLayer, onDaily);
    REGISTER_CALLBACK(MenuLayer, onGarage);
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
