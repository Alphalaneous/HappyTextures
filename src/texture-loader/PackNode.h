#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCScene.hpp>
#include "../NodeModding.h"
#include "PackSettingsPopup.h"
#include "../Config.h"
#include "Pack.h"

using namespace geode::prelude;

class PackSelectLayer;

class PackNode : public CCNode {
protected:
    PackSelectLayer* m_layer;
    std::shared_ptr<Pack> m_pack;
    CCNode* m_draggingBg;

public:
    void onSettings(CCObject* obj) {
        if (m_pack->getInfo().has_value()) {
            PackSettingsPopup* psp = PackSettingsPopup::create(m_pack->getInfo().value());
            psp->show();
        }
    }

    std::shared_ptr<Pack> getPack() { return m_pack; }
};

$execute {
    NodeModding::get()->addNodeToModify("PackNode", [](CCNode* node) {
        
        PackNode* packNode = static_cast<PackNode*>(node);
        if (packNode->getChildByID("author-text")) return;

        CCSprite* settingsSprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
        settingsSprite->setScale(0.4f);

        CCMenuItemSpriteExtra* settingsButton = CCMenuItemSpriteExtra::create(settingsSprite, node, menu_selector(PackNode::onSettings));

        if (CCScale9Sprite* scale9 = node->getChildByType<CCScale9Sprite>(0)) {
            scale9->setContentSize(scale9->getContentSize()/2);
            scale9->setScale(1);
        }

        if (CCMenu* menu = typeinfo_cast<CCMenu*>(node->getChildByID("pack-button-menu"))) {
            menu->setContentSize(node->getContentSize());
            menu->setPosition({0, 0});
            settingsButton->setPosition({node->getContentWidth() - 45, node->getContentHeight()/2 + 1});

            if (CCNode* labelBtn = menu->getChildByID("pack-name-button")) {

                if (CCLabelBMFont* nameLabel = labelBtn->getChildByType<CCLabelBMFont>(0)) {
                    nameLabel->limitLabelWidth(125.f, 0.40f, 0.1f);
                    nameLabel->setPositionX(0);
                    nameLabel->setAnchorPoint({0, 0.5f});
                    labelBtn->setContentWidth(nameLabel->getScaledContentWidth());
                }
                if (!packNode->getPack()->getInfo().has_value()) {
                    settingsButton->setEnabled(false);
                    settingsSprite->setOpacity(127);
                    settingsSprite->setColor({60, 60, 60});
                    labelBtn->setPosition({40 + labelBtn->getContentWidth()/2, node->getContentHeight()/2});
                }
                else {
                    PackInfo packInfo = packNode->getPack()->getInfo().value();
                    CCLabelBMFont* extraInfoLabel = CCLabelBMFont::create(fmt::format("{} | {}", packInfo.m_version.toNonVString(), packInfo.m_id).c_str(), "bigFont.fnt");
                    extraInfoLabel->setColor({127, 127, 127});
                    extraInfoLabel->setScale(0.15f);
                    extraInfoLabel->setAnchorPoint({0, 0.5f});
                    extraInfoLabel->setOpacity(127);
                    extraInfoLabel->setPosition({40, 8});
                    extraInfoLabel->setZOrder(-1);
                    node->addChild(extraInfoLabel);

                    CCLabelBMFont* authorLabel = CCLabelBMFont::create(packInfo.m_authors.at(0).c_str(), "goldFont.fnt");
                    authorLabel->setAnchorPoint({0, 0.5f});
                    authorLabel->setPosition({40.2, 15});
                    authorLabel->setZOrder(-1);
                    authorLabel->setScale(0.25f);
                    node->addChild(authorLabel);

                    labelBtn->setPosition({40 + labelBtn->getContentWidth()/2, node->getContentHeight()-10});
                    if (!Config::get()->hasSettings(packNode->getPack()->getInfo().value().m_id)) {
                        settingsButton->setEnabled(false);
                        settingsSprite->setOpacity(127);
                        settingsSprite->setColor({60, 60, 60});
                    }
                }
            }

            //menu->addChild(settingsButton);
        }
    });
};
