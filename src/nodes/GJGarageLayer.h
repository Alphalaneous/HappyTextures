#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/GJGarageLayer.hpp>
#include "../Callbacks.h"

using namespace geode::prelude;

class $modify(MyGJGarageLayer, GJGarageLayer) {

    void onShards(cocos2d::CCObject* sender) {
        GJGarageLayer::onShards(sender);
        ShardsPage* page = static_cast<ShardsPage*>(getChildByID("ShardsPage"));
        page->removeFromParentAndCleanup(false);
        CCDirector::get()->m_pRunningScene->addChild(page);
    }
};