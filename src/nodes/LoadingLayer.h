#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/LoadingLayer.hpp>
#include "../Macros.h"
#include "../Config.h"

using namespace geode::prelude;

class $modify(MyLoadingLayer, LoadingLayer) {
    
    bool init(bool p0) {
        Utils::clearCaches();
        return LoadingLayer::init(p0);
    }

    void loadAssets() {

        if (m_loadStep > 0) {
            LoadingLayer::loadAssets();
            return;
        }

        UIModding::get()->finishedLoad = false;
        
        queueInMainThread([] {
            Utils::reloadFileNames();
            UIModding::get()->loadNodeFiles();
            Config::get()->loadPackJsons();
        });
        
        LoadingLayer::loadAssets();
    }
};