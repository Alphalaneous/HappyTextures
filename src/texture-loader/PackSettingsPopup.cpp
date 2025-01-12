#include <Geode/Geode.hpp>
#include "PackSettingsPopup.h"
#include "PackNode.h"

PackSettingsPopup* PackSettingsPopup::create(PackInfo pack) {
    auto pRet = new PackSettingsPopup();
    if (pRet && pRet->init(pack)) {
        pRet->autorelease();
        return pRet;
    }
    delete pRet;
    return nullptr;
}

bool PackSettingsPopup::init(PackInfo pack) {

    std::optional<geode::texture_loader::Pack> packOpt = Utils::getPackByID(pack.m_id);
    if (packOpt.has_value()) {
        m_pack = packOpt.value();
    }

    if (!Popup<>::initAnchored(440, 280, "GJ_square01.png")) return false;
    
    setTitle(fmt::format("Settings for {}", pack.m_name));

    return true;
}

bool PackSettingsPopup::isZipped() {
    return !std::filesystem::is_directory(m_pack.path);
}

void PackSettingsPopup::applySettings(CCObject* obj) {

}

void PackSettingsPopup::resetAll(CCObject* obj) {

}

void PackSettingsPopup::openFolder(CCObject* obj) {
    if (!isZipped()) {
        geode::utils::file::openFolder(m_pack.resourcesPath);
    }
}

bool PackSettingsPopup::setup() {

    geode::ListBorders* border = geode::ListBorders::create();
    border->setContentSize({346, 200});
    border->setPosition(m_mainLayer->getContentSize()/2);

    m_mainLayer->addChild(border);

    RowLayout* applyLayout = RowLayout::create();

    CCMenu* applyMenu = CCMenu::create();
    applyMenu->setContentSize({150, 18});
    applyMenu->setLayout(applyLayout);
    applyMenu->setPosition({m_mainLayer->getContentSize().width/2, 20});
    m_mainLayer->addChild(applyMenu);

    ButtonSprite* applyButtonSprite = ButtonSprite::create("Apply", 1);
    applyButtonSprite->setScale(0.6f);
    CCMenuItemSpriteExtra* applyButton = CCMenuItemSpriteExtra::create(applyButtonSprite, this, menu_selector(PackSettingsPopup::applySettings));
    applyMenu->addChild(applyButton);
    applyMenu->updateLayout();

    RowLayout* resetLayout = RowLayout::create();

    CCMenu* resetMenu = CCMenu::create();
    resetMenu->setContentSize({90, 18});
    resetMenu->setLayout(resetLayout);
    resetMenu->setPosition({55, 20});
    m_mainLayer->addChild(resetMenu);

    ButtonSprite* resetButtonSprite = ButtonSprite::create("Reset All", 1);
    resetButtonSprite->setScale(0.6f);
    CCMenuItemSpriteExtra* resetButton = CCMenuItemSpriteExtra::create(resetButtonSprite, this, menu_selector(PackSettingsPopup::applySettings));
    resetMenu->addChild(resetButton);
    resetMenu->updateLayout();

    RowLayout* foldersLayout = RowLayout::create();
    foldersLayout->setAxisAlignment(AxisAlignment::End);
    CCMenu* foldersMenu = CCMenu::create();
    foldersMenu->setContentSize({90, 18});
    foldersMenu->setLayout(foldersLayout);
    foldersMenu->setPosition({m_mainLayer->getContentSize().width - 55, 20});
    m_mainLayer->addChild(foldersMenu);

    IconButtonSprite* foldersButtonSprite = IconButtonSprite::create("GJ_button_01.png", CCSprite::createWithSpriteFrameName("gj_folderBtn_001.png"), "", "bigFont.fnt");
    foldersButtonSprite->setScale(0.6f);
    CCMenuItemSpriteExtra* foldersButton = CCMenuItemSpriteExtra::create(foldersButtonSprite, this, menu_selector(PackSettingsPopup::openFolder));
    foldersMenu->addChild(foldersButton);
    foldersMenu->updateLayout();

    if (isZipped()) {
        foldersButtonSprite->setOpacity(127);
        foldersButtonSprite->setColor({60, 60, 60});
        foldersButton->setEnabled(false);
    }

    return true;
}   