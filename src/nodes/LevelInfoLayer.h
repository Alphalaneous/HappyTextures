#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

class $modify(MyLevelInfoLayer, LevelInfoLayer) {

    void onPlay(cocos2d::CCObject* sender) {
        LevelInfoLayer::onPlay(sender);

        if (UIModding::get()->doModify) {
            CCSprite* spriteOuter = getChildOfType<CCSprite>(m_playSprite, 0);
            CCSprite* spriteInner = getChildOfType<CCSprite>(m_playSprite, 1);
            CCSprite* spriteCenter = getChildOfType<CCSprite>(m_playSprite, 2);
            
            if (CCSprite* spr = Utils::getValidSprite("play_loading_outer.png")) {
                spriteOuter->setTexture(spr->getTexture());
                spriteOuter->setTextureRect(spr->getTextureRect());
            }

            if (CCSprite* spr = Utils::getValidSprite("play_loading_center.png")) {
                spriteCenter->setTexture(spr->getTexture());
                spriteCenter->setTextureRect(spr->getTextureRect());
            }

            if (CCSprite* spr = Utils::getValidSprite("play_loading_inner.png")) {
                spriteInner->setTexture(spr->getTexture());
                spriteInner->setTextureRect(spr->getTextureRect());
            }

            if (CCSprite* spr = Utils::getValidSprite("play_loading_progress.png")) {
                spr->setColor(m_progressTimer->getSprite()->getColor());
                m_progressTimer->setSprite(spr);
            }

            Utils::setColorIfExists(spriteOuter, "play-loading-outer");
            Utils::setColorIfExists(spriteCenter, "play-loading-center");
            Utils::setColorIfExists(spriteInner, "play-loading-inner");
            Utils::setColorIfExists(m_progressTimer->getSprite(), "play-loading-progress");   
        }
    }
};