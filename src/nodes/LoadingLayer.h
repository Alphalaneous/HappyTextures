#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/LoadingLayer.hpp>
#include "../Macros.h"
#include "../Config.h"

using namespace geode::prelude;

class $modify(MyLoadingLayer, LoadingLayer) {
    
    static void onModify(auto& self) {
        (void) self.setHookPriorityPost("LoadingLayer::loadAssets", Priority::Late);
    }

    struct Fields {
        bool m_loaded = false;
    };

    void loadAssets() {
        LoadingLayer::loadAssets();
        auto& loaded = m_fields.self()->m_loaded;
        if (!loaded) {
            UIModding::get()->activePackCache.clear();
            Utils::clearCaches();
            Utils::reloadFileNames();
            UIModding::get()->loadNodeFiles();
            Config::get()->loadPackJsons();
            loaded = true;
        }
    }
};