#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CustomMusicCell.hpp>
#include "../Macros.hpp"
#include "../UIModding.hpp"

using namespace geode::prelude;

class $modify(MyCustomMusicCell, CustomMusicCell) {

    static void onModify(auto& self) {
        HOOK_LATEST("CustomMusicCell::loadFromObject");
    }

	struct Fields {
		ccColor3B m_lastBG;
	};

	void loadFromObject(SongInfoObject* p0){
		CustomMusicCell::loadFromObject(p0);
        if (UIModding::get()->doModify) {
            checkBG(0);
            this->schedule(schedule_selector(MyCustomMusicCell::checkBG));
        }
	}

	void checkBG(float dt) {
		CCLayerColor* child = this->getChildByType<CCLayerColor>(0);
		if (child) {
			if (m_fields->m_lastBG != child->getColor()) {
				m_fields->m_lastBG = child->getColor();
                if (child->getColor() == ccColor3B{75,75,75}) {
                    std::optional<ColorData> dataOpt = UIModding::get()->getColors("music-cell-odd");
                    if (dataOpt.has_value()) {
                        ColorData data = dataOpt.value();
                        child->setColor(data.color);
                        child->setOpacity(data.alpha);
                    }
                }
                else if (child->getColor() == ccColor3B{50,50,50}) {
                    std::optional<ColorData> dataOpt = UIModding::get()->getColors("music-cell-even");
                    if (dataOpt.has_value()) {
                        ColorData data = dataOpt.value();
                        child->setColor(data.color);
                        child->setOpacity(data.alpha);
                    }
                }
			}
		}
	}
};