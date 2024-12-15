#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/LoadingLayer.hpp>

using namespace geode::prelude;

class $modify(MyLoadingLayer, LoadingLayer) {
    
    bool init(bool p0) {
        UIModding::get()->finishedLoad = false;
        UIModding::get()->uiCache.clear();
        UIModding::get()->colorCache.clear();
        if (!LoadingLayer::init(p0)) return false;
        UIModding::get()->loadNodeFiles();
        return true;
    }
};