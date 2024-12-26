#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/LoadingLayer.hpp>
#include "../Macros.h"
#include "../Config.h"

using namespace geode::prelude;

class $modify(MyLoadingLayer, LoadingLayer) {
    
    bool init(bool p0) {
        UIModding::get()->finishedLoad = false;
        UIModding::get()->uiCache.clear();
        UIModding::get()->colorCache.clear();
        Utils::clearActivePackCache();

        if (!LoadingLayer::init(p0)) return false;

        queueInMainThread([] {
            //Utils::reloadFileNames();
            UIModding::get()->loadNodeFiles();
            //Config::get()->loadPackJsons();
        });

        return true;
    }
};