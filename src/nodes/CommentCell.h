#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CommentCell.hpp>
#include "../Utils.h"

using namespace geode::prelude;

class $modify(MyCommentCell, CommentCell) {

    struct Fields {
		ccColor3B m_lastBG;
	};

    static void onModify(auto& self) {
        (void) self.setHookPriority("CommentCell::loadFromComment", INT_MIN);
    }

    void loadFromComment(GJComment* p0) {
        CommentCell::loadFromComment(p0);
        if (UIModding::get()->doModify) {
            this->schedule(schedule_selector(MyCommentCell::checkBG));
        }
    }

	void checkBG(float dt) {
        if (CCLayerColor* child = getChildOfType<CCLayerColor>(this, 0)) {
            if (m_fields->m_lastBG != child->getColor()) {
                m_fields->m_lastBG = child->getColor();
                CCLayer* layer = getChildOfType<CCLayer>(this, 1);
                CCScale9Sprite* bg = getChildOfType<CCScale9Sprite>(layer, 0);

                if (child->getColor() == ccColor3B{161,88,44}) {
                    Utils::setColorIfExists(child, "comment-cell-odd");
                    if (bg) {
                        Utils::setColorIfExists(child, "comment-cell-bg-odd");
                    }
                }
                else if (child->getColor() == ccColor3B{194,114,62}) { 
                    Utils::setColorIfExists(child, "comment-cell-even");
                    if (bg) {
                        Utils::setColorIfExists(child, "comment-cell-bg-even");
                    }
                }
                else if (child->getColor() == ccColor3B{156,85,42}) {
                    Utils::setColorIfExists(child, "comment-cell-small-odd");
                }
                else if (child->getColor() == ccColor3B{144,79,39}) {
                    Utils::setColorIfExists(child, "comment-cell-small-even");
                }
            }
        }
    }
};