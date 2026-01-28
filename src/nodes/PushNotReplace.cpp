#include <Geode/Geode.hpp>
#include <Geode/modify/SecretRewardsLayer.hpp>
#include <Geode/modify/GauntletSelectLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/modify/CCDirector.hpp>
#include "../Macros.hpp"
#include "../UIModding.hpp"

using namespace geode::prelude;

class $modify(MyCreatorLayer, CreatorLayer) {

    void onTreasureRoom(CCObject* sender) {
        if (!UIModding::get()->doModify) return CreatorLayer::onTreasureRoom(sender);

        auto ugv5 = GameManager::get()->getUGV("5");
        if (ugv5) {
            return CreatorLayer::onTreasureRoom(sender);
        }
        auto scene = SecretRewardsLayer::scene(false);
        CCDirector::get()->pushScene(CCTransitionFade::create(0.5, scene));
    }

    void onGauntlets(CCObject* sender) {
        if (!UIModding::get()->doModify) return CreatorLayer::onGauntlets(sender);

        auto scene = GauntletSelectLayer::scene(0);
        CCDirector::get()->pushScene(CCTransitionFade::create(0.5, scene));
    }
};

static bool s_pushNotReplace = false;

class $modify(CCDirectorPush, CCDirector) {

    bool replaceScene(CCScene *pScene) {
        if (s_pushNotReplace) {
            CCDirector::pushScene(pScene);
            return true;
        }
        else return CCDirector::replaceScene(pScene);
    }
};

class $modify(MySecretRewardsLayer, SecretRewardsLayer) {

    void onBack(CCObject* sender) {
        if (!UIModding::get()->doModify) return SecretRewardsLayer::onBack(sender);

        s_pushNotReplace = true;
        SecretRewardsLayer::onBack(sender);
        s_pushNotReplace = false;;
    }
};

class $modify(MyGauntletSelectLayer, GauntletSelectLayer) {

    void onBack(CCObject* sender) {
        if (!UIModding::get()->doModify) return GauntletSelectLayer::onBack(sender);

        s_pushNotReplace = true;
        GauntletSelectLayer::onBack(sender);
        s_pushNotReplace = false;
    }
};
