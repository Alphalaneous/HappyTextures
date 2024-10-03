#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/GJChestSprite.hpp>

using namespace geode::prelude;

class $modify(MyGJChestSprite, GJChestSprite) {

    static void onModify(auto& self) {
        (void) self.setHookPriority("GJChestSprite::switchToState", INT_MIN);
    }

    static CCNode* getChildBySpriteFrameName(CCNode* parent, const char* name, int index = 0) {
        int idx = 0;
        for (auto child : CCArrayExt<CCNode*>(parent->getChildren())) {
            if (::isSpriteFrameName(static_cast<CCNode*>(child), name)) {
                if (idx == index) return child;
                idx++;
            }
        }
        return nullptr;
    }

    void switchToState(ChestSpriteState p0, bool p1) {

        GJChestSprite::switchToState(p0, p1);

        if (UIModding::get()->doModify) {

            if (p0 == ChestSpriteState::Opened) {

                auto glow0 = static_cast<CCSprite*>(getChildBySpriteFrameName(this, "chest_glow_bg_001.png"));
                auto glow1 = static_cast<CCSprite*>(getChildBySpriteFrameName(this, "chest_glow_bg_001.png", 1));
                auto square = static_cast<CCSprite*>(getChildBySpriteFrameName(this, "block005b_05_001.png"));

                if (!glow0 || !glow1 || !square) return;

                std::optional<ColorData> dataOpt;

                ccColor3B color = glow0->getColor();
                if (color == ccColor3B{51, 68, 153}) {
                    dataOpt = UIModding::get()->getColors("chest-opened-overlay");
                }
                if (color == ccColor3B{10, 54, 83}) {
                    dataOpt = UIModding::get()->getColors("25-chest-opened-overlay");
                }
                if (color == ccColor3B{41, 81, 59}) {
                    dataOpt = UIModding::get()->getColors("50-chest-opened-overlay");
                }
                if (color == ccColor3B{58, 27, 75}) {
                    dataOpt = UIModding::get()->getColors("100-chest-opened-overlay");
                }

                if (dataOpt.has_value()) {
                    ColorData data = dataOpt.value();

                    glow0->setColor(data.color);
                    glow1->setColor(data.color);
                    square->setColor(data.color);

                    glow0->setOpacity(data.alpha);
                    glow1->setOpacity(data.alpha);
                    square->setOpacity(data.alpha);
                }
            }
        }
    }
};