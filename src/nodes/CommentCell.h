#pragma once

#ifndef MYCOMMENTCELL_H
#define MYCOMMENTCELL_H

#include <Geode/Geode.hpp>
#include <Geode/modify/CommentCell.hpp>

using namespace geode::prelude;

class $modify(MyCommentCell, CommentCell) {

    struct Fields {
		ccColor3B m_lastBG;
	};

    static void onModify(auto& self) {
        (void) self.setHookPriority("CommentCell::loadFromComment", INT_MIN);
    }

    void loadFromComment(GJComment* p0){
        CommentCell::loadFromComment(p0);
        this->schedule(schedule_selector(MyCommentCell::checkBG));
    }

	void checkBG(float dt) {
        CCLayerColor* child = getChildOfType<CCLayerColor>(this, 0);
        if(child){
            if(m_fields->m_lastBG != child->getColor()){
                m_fields->m_lastBG = child->getColor();
                if(UIModding::get()->doModify){
                    CCLayer* layer = getChildOfType<CCLayer>(this, 1);
                    CCScale9Sprite* bg = getChildOfType<CCScale9Sprite>(layer, 0);

                    if(child->getColor() == ccColor3B{161,88,44}){
                        std::optional<ColorData> dataOpt = UIModding::get()->getColors("comment-cell-odd");
                        if(dataOpt.has_value()){
                            ColorData data = dataOpt.value();
                            child->setColor(data.color);
                            child->setOpacity(data.alpha);
                        }
                        if(bg){
                            std::optional<ColorData> dataOpt2 = UIModding::get()->getColors("comment-cell-bg-odd");
                            if(dataOpt2.has_value()){
                                ColorData data = dataOpt2.value();
                                bg->setColor(data.color);
                                bg->setOpacity(data.alpha);
                            }
                        }
                    }
                    else if(child->getColor() == ccColor3B{194,114,62}){
                        std::optional<ColorData> dataOpt = UIModding::get()->getColors("comment-cell-even");
                        if(dataOpt.has_value()){
                            ColorData data = dataOpt.value();
                            child->setColor(data.color);
                            child->setOpacity(data.alpha);
                        }
                        if(bg){
                            std::optional<ColorData> dataOpt2 = UIModding::get()->getColors("comment-cell-bg-even");
                            if(dataOpt2.has_value()){
                                ColorData data = dataOpt2.value();
                                bg->setColor(data.color);
                                bg->setOpacity(data.alpha);
                            }
                        }
                    }
                    else if(child->getColor() == ccColor3B{156,85,42}){
                        std::optional<ColorData> dataOpt = UIModding::get()->getColors("comment-cell-small-odd");
                        if(dataOpt.has_value()){
                            ColorData data = dataOpt.value();
                            child->setColor(data.color);
                            child->setOpacity(data.alpha);
                        }
                    }
                    else if(child->getColor() == ccColor3B{144,79,39}){
                        std::optional<ColorData> dataOpt = UIModding::get()->getColors("comment-cell-small-even");
                        if(dataOpt.has_value()){
                            ColorData data = dataOpt.value();
                            child->setColor(data.color);
                            child->setOpacity(data.alpha);
                        }
                    }
                }
            }
        }
    }
};

#endif