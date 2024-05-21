#pragma once

#ifndef MYCCDIRECTOR_H
#define MYCCDIRECTOR_H

#include <Geode/Geode.hpp>
#include <Geode/modify/CCDirector.hpp>
#include "UIModding.h"

using namespace geode::prelude;

class $modify(CCDirector){

    static void onModify(auto& self) {
        (void) self.setHookPriority("CCDirector::willSwitchToScene", INT_MIN);
    }

    void willSwitchToScene(CCScene* scene){
        if(UIModding::get()->doModify){
            if(CCSprite* bg = dynamic_cast<CCSprite*>(scene->getChildByIDRecursive("background"))){
                if(bg->getColor() == ccColor3B{0, 102, 255}){
                    std::optional<ColorData> dataOpt = UIModding::get()->getColors("background");
                    if(dataOpt.has_value()){
                        ColorData data = dataOpt.value();
                        bg->setColor(data.color);
                        bg->setOpacity(data.alpha);
                    }
                }
            }
        }
        CCDirector::willSwitchToScene(scene);
    }
};

#endif