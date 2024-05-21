#pragma once

#ifndef MYMENULAYER_H
#define MYMENULAYER_H

#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

class $modify(MyMenuLayer, MenuLayer){

    static void onModify(auto& self) {
        (void) self.setHookPriority("MenuLayer::init", INT_MIN);
    }

    bool init(){

        if(!MenuLayer::init()){
            return false;
        }
        UIModding::get()->doModify = Mod::get()->getSettingValue<bool>("ui-modifications");

        if(UIModding::get()->doModify){
            UIModding::get()->startFileListeners();
            UIModding::get()->doUICheck(this);
        }
        return true;
    }
};

#endif