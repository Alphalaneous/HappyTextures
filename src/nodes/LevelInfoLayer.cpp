#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include "../Macros.hpp"
#include "../UIModding.hpp"
#include "../Utils.hpp"
#include "alphalaneous.alphas_geode_utils/include/Utils.hpp"

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

            if (auto res = alpha::utils::cocos::getSprite("play_loading_outer.png")) {
                auto spr = res.value();
                spriteOuter->setTexture(spr->getTexture());
                spriteOuter->setTextureRect(spr->getTextureRect());
            }

            if (auto res = alpha::utils::cocos::getSprite("play_loading_center.png")) {
                auto spr = res.value();
                spriteCenter->setTexture(spr->getTexture());
                spriteCenter->setTextureRect(spr->getTextureRect());
            }

            if (auto res = alpha::utils::cocos::getSprite("play_loading_inner.png")) {
                auto spr = res.value();
                spriteInner->setTexture(spr->getTexture());
                spriteInner->setTextureRect(spr->getTextureRect());
            }

            if (auto res = alpha::utils::cocos::getSprite("play_loading_progress.png")) {
                auto spr = res.value();
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