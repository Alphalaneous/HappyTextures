#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CommentCell.hpp>
#include "../Utils.hpp"
#include "../Macros.hpp"
#include "../UIModding.hpp"

using namespace geode::prelude;

class $modify(MyCommentCell, CommentCell) {

    struct Fields {
		ccColor3B m_lastBG;
	};

    static void onModify(auto& self) {
        HOOK_LATEST("CommentCell::loadFromComment");
    }

    void loadFromComment(GJComment* p0) {
        CommentCell::loadFromComment(p0);
        if (UIModding::get()->doModify) {
            checkBG(0);
            this->schedule(schedule_selector(MyCommentCell::checkBG));
        }
    }

	void checkBG(float dt) {
        auto fields = m_fields.self();
        auto color = m_backgroundLayer->getColor();

        if (fields->m_lastBG == color) return;
        fields->m_lastBG = color;

        auto bg = typeinfo_cast<CCScale9Sprite*>(this->getChildByIDRecursive("background"));

        struct ColorInfo {
            ccColor3B match;
            const char* layerID;
            const char* bgID;
        };

        constexpr ColorInfo colorMap[] = {
            {{161,  88,  44},  "comment-cell-odd",        "comment-cell-bg-odd"},
            {{194, 114,  62},  "comment-cell-even",       "comment-cell-bg-even"},
            {{156,  85,  42},  "comment-cell-small-odd",  nullptr},
            {{144,  79,  39},  "comment-cell-small-even", nullptr},
        };

        for (const auto& entry : colorMap) {
            if (color == entry.match) {
                Utils::setColorIfExists(m_backgroundLayer, entry.layerID);
                if (entry.bgID && bg) {
                    Utils::setColorIfExists(bg, entry.bgID);
                }
                break;
            }
        }
    }
};