#pragma once

#ifndef MYGJDROPDOWNLAYER_H
#define MYGJDROPDOWNLAYER_H

#include <Geode/Geode.hpp>
#include <Geode/modify/GJDropDownLayer.hpp>

using namespace geode::prelude;

class $modify(MyGJDropDownLayer, GJDropDownLayer){

    static void onModify(auto& self) {
        (void) self.setHookPriority("GJDropDownLayer::showLayer", INT_MIN);
    }

    void showLayer(bool p0){
        GJDropDownLayer::showLayer(p0);
        if(UIModding::get()->doModify){
            UIModding::get()->doUICheck(this);
        }
    }
};

#endif