#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CustomListView.hpp>
#include "../Macros.hpp"
#include "../Utils.hpp"
#include <alphalaneous.alphas_geode_utils/include/NodeModding.h>

using namespace geode::prelude;

class $modify(MyCustomListView, CustomListView) {

    void setupList(float p0) {
        CustomListView::setupList(p0);
        for (auto cell : CCArrayExt<TableViewCell*>(m_tableView->m_cellArray)) {
            if (typeinfo_cast<CommentCell*>(cell)) {
                setBGComment(cell);
            }
            else if (typeinfo_cast<CustomMusicCell*>(cell)) {
                setBGMusic(cell);
            }
            else {
                setBGCommon(cell);
            }
        }
    }

    void setBGMusic(TableViewCell* node) {
        if (node->m_backgroundLayer->getColor() == ccColor3B{75,75,75}) {
            Utils::setColorIfExists(node->m_backgroundLayer, "music-cell-odd");
        }
        else if (node->m_backgroundLayer->getColor() == ccColor3B{50,50,50}) {
            Utils::setColorIfExists(node->m_backgroundLayer, "music-cell-even");
        }
	}

    void setBGComment(TableViewCell* node) {
        auto color = node->m_backgroundLayer->getColor();

        auto bg = typeinfo_cast<CCScale9Sprite*>(node->getChildByIDRecursive("background"));

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
                Utils::setColorIfExists(node->m_backgroundLayer, entry.layerID);
                if (entry.bgID && bg) {
                    Utils::setColorIfExists(bg, entry.bgID);
                }
                break;
            }
        }
    }

    void setBGCommon(TableViewCell* node) {
        auto color = node->m_backgroundLayer->getColor();

        struct ColorInfo {
            ccColor3B match;
            const char* ID;
        };

        constexpr ColorInfo mappings[] = {
            {{161,  88,  44}, "list-cell-odd"},
            {{194, 114,  62}, "list-cell-even"},
            {{230, 150,  10}, "list-cell-selected"},
        };

        for (const auto& entry : mappings) {
            if (color == entry.match) {
                Utils::setColorIfExists(node->m_backgroundLayer, entry.ID);
                break;
            }
        }
    } 
};