#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include "../Macros.hpp"
#include "../UIModding.hpp"
#include "../Utils.hpp"

using namespace geode::prelude;

class $modify(MyLevelInfoLayer, LevelInfoLayer) {

    static void onModify(auto& self) {
        HOOK_LATEST("LevelInfoLayer::loadLevelStep");
    }

    struct Fields {
        bool m_didLoad;
    };

    void loadLevelStep() {
        LevelInfoLayer::loadLevelStep();

        if (UIModding::get()->doModify) {

            if (!m_playSprite) return;
            
            if (m_fields->m_didLoad) return;
            m_fields->m_didLoad = true;

            CCSprite* spriteOuter = m_playSprite->getChildByType<CCSprite>(0);
            CCSprite* spriteInner = m_playSprite->getChildByType<CCSprite>(1);
            CCSprite* spriteCenter = m_playSprite->getChildByType<CCSprite>(2);
            
            if (!spriteOuter || !spriteInner || !spriteCenter || !m_progressTimer || !m_progressTimer->getSprite()) return;

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