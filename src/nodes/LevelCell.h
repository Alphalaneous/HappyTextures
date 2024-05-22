#pragma once

#ifndef MYLEVELCELL_H
#define MYLEVELCELL_H

#include <Geode/Geode.hpp>
#include <Geode/modify/LevelCell.hpp>

using namespace geode::prelude;

class $modify(MyLevelCell, LevelCell) {

    static void onModify(auto& self) {
        (void) self.setHookPriority("LevelCell::updateBGColor", INT_MIN);
    }

    #ifndef GEODE_IS_MACOS

	void updateBGColor(int p0) {

		LevelCell::updateBGColor(p0);

        if(UIModding::get()->doModify){
            CCLayerColor* child = getChildOfType<CCLayerColor>(this, 0);

            if(child->getColor() == ccColor3B{161,88,44}){
                std::optional<ColorData> dataOpt = UIModding::get()->getColors("list-cell-odd");
                if(dataOpt.has_value()){
                    ColorData data = dataOpt.value();
                    child->setColor(data.color);
                    child->setOpacity(data.alpha);
                }
            }
            else if(child->getColor() == ccColor3B{194,114,62}){
                std::optional<ColorData> dataOpt = UIModding::get()->getColors("list-cell-even");
                if(dataOpt.has_value()){
                    ColorData data = dataOpt.value();
                    child->setColor(data.color);
                    child->setOpacity(data.alpha);
                }
            }
        }
	}

    #endif
};

#endif