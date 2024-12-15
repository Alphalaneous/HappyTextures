#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/LoadingLayer.hpp>

using namespace geode::prelude;

class $modify(MyLoadingLayer, LoadingLayer) {
    
    bool init(bool p0) {
        UIModding::get()->finishedLoad = false;
        UIModding::get()->uiCache.clear();
        UIModding::get()->colorCache.clear();
        UIModding::get()->loadNodeFiles();
        return LoadingLayer::init(p0);
    }
};