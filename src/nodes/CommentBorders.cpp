#include <Geode/Geode.hpp>
#include "CCScale9Sprite.hpp"
#include "../Macros.hpp"
#include "../UIModding.hpp"
#include "../Utils.hpp"
#include <alphalaneous.alphas_geode_utils/include/ObjectModify.hpp>
#include <Geode/modify/ProfilePage.hpp>
#include <Geode/modify/InfoLayer.hpp>
#include <Geode/modify/LevelLeaderboard.hpp>
#include <Geode/modify/AudioAssetsBrowser.hpp>
#include <Geode/modify/MusicBrowser.hpp>
#include <Geode/modify/OptionsScrollLayer.hpp>
#include <Geode/modify/FRequestProfilePage.hpp>
#include <Geode/modify/FriendsProfilePage.hpp>
#include <Geode/modify/MessagesProfilePage.hpp>
#include <Geode/modify/SFXBrowser.hpp>

using namespace geode::prelude;

class $classModify(MyGJCommentListLayer, GJCommentListLayer) {

    struct Fields {
        SEL_SCHEDULE m_posSchedule;
        bool m_hasBorder = false;
        bool m_setBorder = false;
        CCPoint m_lastPos;
    };

    void modify() {
        if (UIModding::get()->doModify) {
            if (getColor() == ccColor3B{191,114,62}) {
                Utils::setColorIfExists(this, "comment-list-layer-bg");
            }
        }
    }

    void setBorder() {
        bool doFix = Mod::get()->getSettingValue<bool>("comment-border-fix");
        auto fields = m_fields.self();

        if (doFix && !fields->m_setBorder) {
            fields->m_setBorder = true;
            bool brownBorder = true;

            if (auto node = typeinfo_cast<CCSprite*>(getChildByID("left-border"))) {
                brownBorder = Utils::getSpriteName(node) == "GJ_commentSide_001.png";
                node->setVisible(false);
            }
            if (auto node = getChildByID("right-border")) {
                node->setVisible(false);
            }
            if (auto node = getChildByID("top-border")) {
                node->setVisible(false);
            }
            if (auto node = getChildByID("bottom-border")) {
                node->setVisible(false);
            }
        
            fields->m_posSchedule = schedule_selector(MyGJCommentListLayer::listenForPosition);
            
            if (!getUserFlag("dont-correct-borders"_spr)) {
                updateBordersWithParent(getParent());
            }

            schedule(fields->m_posSchedule);

            auto size = getContentSize();
    
            auto pos = CCPoint{size.width/2, size.height/2};

            auto outlineSprite = CCScale9Sprite::create("commentBorder.png"_spr);
            outlineSprite->setContentSize({size.width + 1.6f, size.height + 1.6f});
            outlineSprite->setPosition({pos.x, pos.y});
            outlineSprite->setZOrder(20);
            outlineSprite->setID("outline"_spr);

            if (brownBorder) {
                outlineSprite->setColor({130, 64, 32});
                std::optional<ColorData> dataOpt = UIModding::get()->getColors("comment-list-outline-brown");
                if (dataOpt.has_value()) {
                    ColorData data = dataOpt.value();
                    outlineSprite->setColor(data.color);
                }
            }
            else {
                outlineSprite->setColor({32, 49, 130});
                std::optional<ColorData> dataOpt = UIModding::get()->getColors("comment-list-outline-blue");
                if (dataOpt.has_value()) {
                    ColorData data = dataOpt.value();
                    outlineSprite->setColor(data.color);
                }
            }
            
            addChild(outlineSprite);
        }
    }

    void listenForDisable(float dt) {
        if (getUserFlag("dont-correct-borders"_spr)){
            revert();
        }
    }

    void revert(bool showBorders) {        
        unschedule(m_fields->m_posSchedule);

        if (showBorders) {
            if (auto node = getChildByID("left-border")) {
                node->setVisible(true);
            }
            if (auto node = getChildByID("right-border")) {
                node->setVisible(true);
            }
            if (auto node = getChildByID("top-border")) {
                node->setVisible(true);
            }
            if (auto node = getChildByID("bottom-border")) {
                node->setVisible(true);
            }
        }

        removeChildByID("outline"_spr);
        removeChildByID("special-border"_spr);
    }

    void revert() {
        revert(true);
    }

    void listenForPosition(float dt) {
        auto fields = m_fields.self();
        if (fields->m_hasBorder && fields->m_lastPos != getPosition()) {
            if (auto parent = getParent()) {
                updateBordersWithParent(parent);
            }
            fields->m_lastPos = getPosition();
        }
    }

    void updateBordersWithParent(CCNode* parent) {
        if (!parent) return;

        if (auto bg = parent->getChildByID("background")) {
            if (bg->getChildByType<CCLayerGradient>(0)) {
                revert(false);
                return;
            }
        }

        if (parent->getChildByID("bitz.customprofiles/normal-gradient")) {
            revert(false);
            return;
        }

        if (parent->getChildByID("thesillydoggo.gradientpages/gradient-container")) {
            revert(false);
            return;
        }

        if (auto bg = typeinfo_cast<CCScale9Sprite*>(parent->getChildByID("background"))) {
            createMask(bg);
        }
        else if (auto bg = parent->getChildByType<CCScale9Sprite>(0)) {
            createMask(bg);
        }
        else {
            auto newBg = CCScale9Sprite::create("GJ_square01.png");
            newBg->setContentSize({440, 290});
            newBg->setPosition(parent->getContentSize() / 2);
            createMask(newBg);
        }
    }

    void createMask(CCScale9Sprite* bg) {

        removeChildByID("special-border"_spr);
        m_fields->m_hasBorder = true;

        auto winSize = CCDirector::get()->getWinSize();
        auto nodeSize = getContentSize();
        auto nodePos = getPosition();
        auto innerPos = CCPoint{nodePos.x + nodeSize.width/2, nodePos.y + nodeSize.height/2};

        auto innerCurve = geode::NineSlice::create("borderStencil.png"_spr);
        innerCurve->setContentSize({nodeSize.width + 5, nodeSize.height + 5});
        innerCurve->setPosition(innerPos);
        innerCurve->setZOrder(20);

        auto myBG = static_cast<MyCCScale9Sprite*>(bg);

        auto newBG = CCScale9Sprite::create(myBG->m_fields->textureName.c_str(), myBG->m_fields->rect, myBG->m_fields->capInsets);
        if (newBG) {
            newBG->setContentSize(myBG->getContentSize());
            newBG->setPosition(bg->getPosition());

            auto parentNode = CCNode::create();

            auto clippingNode = CCClippingNode::create();
            clippingNode->addChild(newBG);
            clippingNode->setStencil(innerCurve);
            clippingNode->setContentSize(winSize);
            clippingNode->setPosition(-nodePos.x + 5, -nodePos.y + 5);
            clippingNode->setAnchorPoint({0, 0});
            clippingNode->setAlphaThreshold(0.7f);

            parentNode->setAnchorPoint({0, 0});
            parentNode->setContentSize({nodeSize.width+10, nodeSize.height+10});
            parentNode->setPosition({-5, -5});
            parentNode->setID("special-border"_spr);
            parentNode->setZOrder(19);
            parentNode->addChild(clippingNode);

            addChild(parentNode);
        }
    }
};

class $modify(ProfilePage) {
    void setBorder() {
        for (auto child : m_mainLayer->getChildrenExt()) {
            if (auto list = typeinfo_cast<GJCommentListLayer*>(child)) {
                static_cast<MyGJCommentListLayer*>(list)->setBorder();
            }
        }
    }

    void setupCommentsBrowser(cocos2d::CCArray* comments) {
        ProfilePage::setupCommentsBrowser(comments);
        setBorder();
    }

    void loadPageFromUserInfo(GJUserScore* score) {
        ProfilePage::loadPageFromUserInfo(score);
        setBorder();
    }
};

class $modify(InfoLayer) {

    void setupCommentsBrowser(cocos2d::CCArray* comments) {
        InfoLayer::setupCommentsBrowser(comments);
        static_cast<MyGJCommentListLayer*>(m_list)->setBorder();
    }
};

class $modify(LevelLeaderboard) {
    
    void setupLeaderboard(cocos2d::CCArray* scores) {
        LevelLeaderboard::setupLeaderboard(scores);
        static_cast<MyGJCommentListLayer*>(m_list)->setBorder();
    }
};

class $modify(AudioAssetsBrowser) {
    
    void setupList() {
        AudioAssetsBrowser::setupList();
        static_cast<MyGJCommentListLayer*>(m_songList)->setBorder();
    }
};

class $modify(MusicBrowser) {
    
    void setupList(MusicSearchResult* result) {
        MusicBrowser::setupList(result);
        static_cast<MyGJCommentListLayer*>(m_listLayer)->setBorder();
    }
};

class $modify(OptionsScrollLayer) {
    
    void setupList(cocos2d::CCArray* objects) {
        OptionsScrollLayer::setupList(objects);
        static_cast<MyGJCommentListLayer*>(m_listLayer)->setBorder();
    }
};

class $modify(MessagesProfilePage) {
    
    void setupCommentsBrowser(cocos2d::CCArray* messages) {
        MessagesProfilePage::setupCommentsBrowser(messages);
        static_cast<MyGJCommentListLayer*>(m_listLayer)->setBorder();
    }
};

class $modify(FriendsProfilePage) {
    
    void setupUsersBrowser(cocos2d::CCArray* users, UserListType type) {
        FriendsProfilePage::setupUsersBrowser(users, type);
        static_cast<MyGJCommentListLayer*>(m_listLayer)->setBorder();
    }
};

class $modify(FRequestProfilePage) {
    
    void setupCommentsBrowser(cocos2d::CCArray* scores) {
        FRequestProfilePage::setupCommentsBrowser(scores);
        static_cast<MyGJCommentListLayer*>(m_listLayer)->setBorder();
    }
};

class $modify(SFXBrowser) {
    
    void setupList(SFXSearchResult* result) {
        SFXBrowser::setupList(result);
        static_cast<MyGJCommentListLayer*>(m_listLayer)->setBorder();
    }
};