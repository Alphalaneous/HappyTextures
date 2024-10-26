#include "AlertDelegate.h"
#include "../nodes/CCLabelBMFont.h"

bool AlertDelegate::init(float w, float h, const char* spr, const char* title) {

    auto winSize = CCDirector::sharedDirector()->getWinSize();
    this->m_pLrSize = CCSize { w, h };

    if (!this->initWithColor({ 0, 0, 0, 105 })) return false;
    this->m_mainLayer = CCLayer::create();
    this->m_mainLayer->setID("main-layer");
    this->addChild(this->m_mainLayer);

    auto bg = extension::CCScale9Sprite::create(spr, { 0.0f, 0.0f, 80.0f, 80.0f });
    bg->setContentSize(this->m_pLrSize);
    bg->setPosition(winSize.width / 2, winSize.height / 2);
    this->m_mainLayer->addChild(bg);
    this->m_pBGSprite = bg;
    this->m_pBGSprite->setID("background");
    this->m_buttonMenu = CCMenu::create();
    this->m_mainLayer->addChild(this->m_buttonMenu);

    if (strlen(title) > 0) {
        MyCCLabelBMFont* titleLabel = static_cast<MyCCLabelBMFont*>(CCLabelBMFont::create(title, "bigFont.fnt"));
        titleLabel->setHappyTexturesModified(true);
        titleLabel->limitLabelWidth(this->m_pLrSize.width * 4, .8f, .2f);
        titleLabel->setPosition(winSize.width / 2, winSize.height / 2 + this->m_pLrSize.height / 2 - 25);

        this->m_mainLayer->addChild(titleLabel);
    }

    this->registerWithTouchDispatcher();
    
    auto closeSpr = CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    closeSpr->setScale(1.0f);

    auto closeBtn = CCMenuItemSpriteExtra::create(closeSpr, this, menu_selector(AlertDelegate::onClose));
    closeBtn->setUserData(reinterpret_cast<void*>(this));

    this->m_buttonMenu->addChild(closeBtn);
    this->m_buttonMenu->setID("main-menu");
    closeBtn->setPosition( -w / 2, h / 2 );
    closeBtn->setID("close-button");
    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);

    CCTouchDispatcher::get()->registerForcePrio(this, 2);

    return true;
}

void AlertDelegate::keyDown(enumKeyCodes key) {
    if (key == enumKeyCodes::KEY_Escape)
        return onClose(nullptr);
    if (key == enumKeyCodes::KEY_Space)
        return;
    
    return FLAlertLayer::keyDown(key);
}

void AlertDelegate::onClose(CCObject* pSender) {
    this->setKeyboardEnabled(false);
    this->setTouchEnabled(false);
    this->removeFromParentAndCleanup(true);
};
