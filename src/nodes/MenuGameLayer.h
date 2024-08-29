#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/MenuGameLayer.hpp>

using namespace geode::prelude;

class $modify(MyMenuGameLayer, MenuGameLayer){

    static void onModify(auto& self) {
        (void) self.setHookPriority("MenuGameLayer::update", INT_MIN);
    }

    void updateGroundSprite(CCSprite* spr){
        std::optional<ColorData> dataOpt = UIModding::get()->getColors("main-menu-ground");
        if(dataOpt.has_value()){
            for(CCNode* node : CCArrayExt<CCNode*>(spr->getChildren())){
                if(CCSprite* child = typeinfo_cast<CCSprite*>(node)){
                    ColorData data = dataOpt.value();
                    child->setColor(data.color);
                    child->setOpacity(data.alpha);
                }
            }
            ColorData data = dataOpt.value();
            spr->setColor(data.color);
            spr->setOpacity(data.alpha);
        }
    }

    void update(float p0){
        MenuGameLayer::update(p0);

        if(UIModding::get()->doModify){

            CCSprite* bg = typeinfo_cast<CCSprite*>(getChildByID("background"));
            GJGroundLayer* ground = typeinfo_cast<GJGroundLayer*>(getChildByID("ground"));

            if(bg){
                std::optional<ColorData> dataOpt = UIModding::get()->getColors("main-menu-bg");
                if(dataOpt.has_value()){
                    ColorData data = dataOpt.value();
                    bg->setColor(data.color);
                    bg->setOpacity(data.alpha);
                }
            }
            if(ground){
                if(CCSpriteBatchNode* groundSprites = typeinfo_cast<CCSpriteBatchNode*>(ground->getChildByID("ground-sprites"))){
                    if(CCSprite* groundSprite = getChildOfType<CCSprite>(groundSprites, 0)) {
                        updateGroundSprite(groundSprite);
                    }
                }
                if(CCSpriteBatchNode* groundSprites2 = typeinfo_cast<CCSpriteBatchNode*>(ground->getChildByID("ground-sprites-2"))){
                    if(CCSprite* groundSprite = getChildOfType<CCSprite>(groundSprites2, 0)) {
                        updateGroundSprite(groundSprite);
                    }
                }
            }
        }
    }
};