#pragma once

#ifndef __BROWNALERT_HPP
#define __BROWNALERT_HPP

#include <Geode/Geode.hpp>

using namespace geode::prelude;
class AlertDelegate : public FLAlertLayer {
    protected:
    
        CCSize m_pLrSize;
        CCScale9Sprite* m_pBGSprite;

        virtual bool init(float width, float height, const char* bg, const char* title);
        virtual void keyDown(enumKeyCodes) override;

        virtual void onClose(CCObject*);

        ~AlertDelegate() override {
            CCTouchDispatcher::get()->unregisterForcePrio(this);
        }
};

#endif
