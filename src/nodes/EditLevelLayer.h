#pragma once

#ifndef MYEDITLEVELLAYER_H
#define MYEDITLEVELLAYER_H

#include <Geode/Geode.hpp>
#include <Geode/modify/EditLevelLayer.hpp>

using namespace geode::prelude;

class $modify(MyEditLevelLayer, EditLevelLayer){

    static void onModify(auto& self) {
        (void) self.setHookPriority("EditLevelLayer::init", INT_MIN);
    }

    bool init(GJGameLevel* p0){
        if(!EditLevelLayer::init(p0)) return false;
        if(UIModding::get()->doModify){
            if(CCScale9Sprite* bg = typeinfo_cast<CCScale9Sprite*>(this->getChildByID("description-background"))){
            std::optional<ColorData> dataOpt = UIModding::get()->getColors("edit-description-bg");
                if(dataOpt.has_value()){
                    ColorData data = dataOpt.value();
                    bg->setColor(data.color);
                    bg->setOpacity(data.alpha);
                }
            }
            if(CCScale9Sprite* bg = typeinfo_cast<CCScale9Sprite*>(this->getChildByID("level-name-background"))){
            std::optional<ColorData> dataOpt = UIModding::get()->getColors("edit-name-bg");
                if(dataOpt.has_value()){
                    ColorData data = dataOpt.value();
                    bg->setColor(data.color);
                    bg->setOpacity(data.alpha);
                }
            }
        }
        return true;
    }
};

#endif