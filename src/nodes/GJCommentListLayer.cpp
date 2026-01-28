#include <Geode/Geode.hpp>
#include "CCScale9Sprite.hpp"
#include "../Macros.hpp"
#include "../UIModding.hpp"
#include "../Utils.hpp"
#include <alphalaneous.alphas_geode_utils/include/ObjectModify.hpp>
#include "../LateQueue.hpp"

using namespace geode::prelude;

class $classModify(MyGJCommentListLayer, GJCommentListLayer) {

    struct Fields {
        SEL_SCHEDULE revertSchedule;
        SEL_SCHEDULE posSchedule;
        bool hasBorder = false;
        CCPoint lastPos;
    };

    void modify() {
        if (UIModding::get()->doModify) {
            if (getColor() == ccColor3B{191,114,62}) {
                Utils::setColorIfExists(this, "comment-list-layer-bg");
            }
        }

        bool doFix = Mod::get()->getSettingValue<bool>("comment-border-fix");

        if (doFix) {
            bool brownBorder = true;
            if (CCSprite* node = typeinfo_cast<CCSprite*>(getChildByID("left-border"))) {
                brownBorder = Utils::getSpriteName(node) == "GJ_commentSide_001.png";
                node->setVisible(false);
            }
            if (CCNode* node = getChildByID("right-border")) {
                node->setVisible(false);
            }
            if (CCNode* node = getChildByID("top-border")) {
                node->setVisible(false);
            }
            if (CCNode* node = getChildByID("bottom-border")) {
                node->setVisible(false);
            }
        
            auto fields = m_fields.self();

            fields->posSchedule = schedule_selector(MyGJCommentListLayer::listenForPosition);
            fields->revertSchedule = schedule_selector(MyGJCommentListLayer::listenForDisable);

            LateQueue::get()->queue(this, [this] {
                if (!static_cast<CCNode*>(this)->getUserFlag("dont-correct-borders"_spr)){
                    updateBordersWithParent(getParent());
                }
            });

            schedule(fields->posSchedule);
            schedule(fields->revertSchedule);

            CCSize size = getContentSize();
    
            CCPoint pos = {size.width/2, size.height/2};

            CCScale9Sprite* outlineSprite = CCScale9Sprite::create("commentBorder.png"_spr);
            outlineSprite->setContentSize({size.width + 1.6f, size.height + 1.6f});
            outlineSprite->setPosition({pos.x, pos.y});
            outlineSprite->setZOrder(20);
            outlineSprite->setID("outline");

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
        if (static_cast<CCNode*>(this)->getUserFlag("dont-correct-borders"_spr)){
            revert();
        }
    }

    void revert(bool showBorders) {
        
        unschedule(m_fields->posSchedule);

        if (showBorders) {
            if (CCNode* node = getChildByID("left-border")) {
                node->setVisible(true);
            }
            if (CCNode* node = getChildByID("right-border")) {
                node->setVisible(true);
            }
            if (CCNode* node = getChildByID("top-border")) {
                node->setVisible(true);
            }
            if (CCNode* node = getChildByID("bottom-border")) {
                node->setVisible(true);
            }
        }

        removeChildByID("outline");
        removeChildByID("special-border");

        unschedule(m_fields->revertSchedule);
    }

    void revert() {
        revert(true);
    }

    void listenForPosition(float dt) {
        if (m_fields->hasBorder && m_fields->lastPos != getPosition()) {
            if (CCNode* parent = getParent()) {
                updateBordersWithParent(parent);
            }
            m_fields->lastPos = getPosition();
        }
    }

    void updateBordersWithParent(CCNode* parent) {
        if (!parent) return;

        if (CCNode* bg = parent->getChildByID("background")) {
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

        if (CCScale9Sprite* bg = typeinfo_cast<CCScale9Sprite*>(parent->getChildByID("background"))) {
            createMask(bg);
        }
        else if (CCScale9Sprite* bg = parent->getChildByType<CCScale9Sprite>(0)) {
            createMask(bg);
        }
    }

    void createMask(CCScale9Sprite* bg) {

        removeChildByID("special-border");
        m_fields->hasBorder = true;

        CCSize winSize = CCDirector::get()->getWinSize();
        CCSize nodeSize = getContentSize();
        CCPoint nodePos = getPosition();
        CCPoint innerPos = {nodePos.x + nodeSize.width/2, nodePos.y + nodeSize.height/2};

        CCScale9Sprite* innerCurve = CCScale9Sprite::create("borderStencil.png"_spr);
        innerCurve->setContentSize({nodeSize.width + 5, nodeSize.height + 5});
        innerCurve->setPosition(innerPos);
        innerCurve->setZOrder(20);

        MyCCScale9Sprite* myBG = static_cast<MyCCScale9Sprite*>(bg);

        CCScale9Sprite* newBG = CCScale9Sprite::create(myBG->m_fields->textureName.c_str(), myBG->m_fields->rect, myBG->m_fields->capInsets);
        newBG->setContentSize(myBG->getContentSize());
        newBG->setPosition(bg->getPosition());

        CCNode* parentNode = CCNode::create();

        CCClippingNode* clippingNode = CCClippingNode::create();
        clippingNode->addChild(newBG);
        clippingNode->setStencil(innerCurve);
        clippingNode->setContentSize(winSize);
        clippingNode->setPosition(-nodePos.x + 5, -nodePos.y + 5);
        clippingNode->setAnchorPoint({0, 0});
        clippingNode->setAlphaThreshold(0.7f);

        parentNode->setAnchorPoint({0, 0});
        parentNode->setContentSize({nodeSize.width+10, nodeSize.height+10});
        parentNode->setPosition({-5, -5});
        parentNode->setID("special-border");
        parentNode->setZOrder(19);
        parentNode->addChild(clippingNode);

        addChild(parentNode);
    }
};