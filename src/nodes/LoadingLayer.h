#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/LoadingLayer.hpp>
#include "../Macros.h"
#include "../Config.h"

using namespace geode::prelude;

class $modify(MyLoadingLayer, LoadingLayer) {
    
    void loadAssets() {

        if (m_loadStep > 0) {
            LoadingLayer::loadAssets();
            return;
        }

        UIModding::get()->finishedLoad = false;
        UIModding::get()->uiCache.clear();
        UIModding::get()->colorCache.clear();
        UIModding::get()->textureToNameMap.clear();
        
        queueInMainThread([] {
            Utils::clearActivePackCache();
            Utils::reloadFileNames();
            UIModding::get()->loadNodeFiles();
            Config::get()->loadPackJsons();
        });
        
        LoadingLayer::loadAssets();
    }
};