#pragma once

#ifndef MYMENUGAMELAYER_H
#define MYMENUGAMELAYER_H

#include <Geode/Geode.hpp>
#include <Geode/modify/MenuGameLayer.hpp>

using namespace geode::prelude;

class $modify(MyMenuGameLayer, MenuGameLayer){

    static void onModify(auto& self) {
        (void) self.setHookPriority("MenuGameLayer::update", INT_MIN);
    }

    void update(float p0){
        MenuGameLayer::update(p0);

        CCSprite* bg = getChildOfType<CCSprite>(this, 0);
        GJGroundLayer* ground = getChildOfType<GJGroundLayer>(this, 0);

        if(bg){
            std::optional<ColorData> dataOpt = UIModding::get()->getColors("main-menu-bg");
            if(dataOpt.has_value()){
                ColorData data = dataOpt.value();
                bg->setColor(data.color);
                bg->setOpacity(data.alpha);
            }
        }
        if(ground){
            CCSpriteBatchNode* groundBatch = getChildOfType<CCSpriteBatchNode>(ground, 0);
            if(groundBatch){
                CCSprite* groundSprite = getChildOfType<CCSprite>(groundBatch, 0);
                if(groundSprite){
                    for(CCNode* node : CCArrayExt<CCNode*>(groundSprite->getChildren())){
                        if(CCSprite* child = dynamic_cast<CCSprite*>(node)){
                            std::optional<ColorData> dataOpt = UIModding::get()->getColors("main-menu-ground");
                            if(dataOpt.has_value()){
                                ColorData data = dataOpt.value();
                                child->setColor(data.color);
                                child->setOpacity(data.alpha);
                            }
                        }
                    }
                    std::optional<ColorData> dataOpt = UIModding::get()->getColors("main-menu-ground");
                    if(dataOpt.has_value()){
                        ColorData data = dataOpt.value();
                        groundSprite->setColor(data.color);
                        groundSprite->setOpacity(data.alpha);
                    }
                }
            }
        }
    }
};

#endif