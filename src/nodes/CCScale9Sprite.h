#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCScale9Sprite.hpp>
#include "../Macros.h"

using namespace geode::prelude;

#define setRect(node, idx) node->setTextureRect(m_fields->originalRects.at(idx), node->isTextureRectRotated(), m_fields->originalRects.at(idx).size);

class $modify(MyCCScale9Sprite, CCScale9Sprite) {

    struct Fields {
        std::string textureName;
        CCRect rect;
        CCRect capInsets;
        std::vector<CCRect> originalRects;
        bool setOriginalRects = true;
    };

    bool initWithFile(const char* file, CCRect rect, CCRect capInsets) {
        if(!CCScale9Sprite::initWithFile(file, rect, capInsets)) return false;
        m_fields->textureName = std::string(file);
        m_fields->rect = rect;
        m_fields->capInsets = capInsets;

        return true;
    }

    void visit() {
        bool doFix = Mod::get()->getSettingValue<bool>("ccscale9sprite-fix");

        if (doFix) {
            if (this->*(&MyCCScale9Sprite::m_positionsAreDirty)) {
                updateSprites();
                this->*(&MyCCScale9Sprite::m_positionsAreDirty) = false;
            }
            CCNode::visit();
        }
        else {
            CCScale9Sprite::visit();
        }
    }

    void updateSprites() {

        CCSprite* topLeft = public_cast(this, _topLeft);
        CCSprite* top = public_cast(this, _top);
        CCSprite* topRight = public_cast(this, _topRight);
        CCSprite* left = public_cast(this, _left);
        CCSprite* center = public_cast(this, _centre);
        CCSprite* right = public_cast(this, _right);
        CCSprite* bottomLeft = public_cast(this, _bottomLeft);
        CCSprite* bottom = public_cast(this, _bottom);
        CCSprite* bottomRight = public_cast(this, _bottomRight);

        if (!topLeft || !top || !topRight || !left || !center || !right || !bottomLeft || !bottom || !bottomRight) return;

        topLeft->setID("top-left");
        top->setID("top");
        topRight->setID("top-right");
        left->setID("left");
        center->setID("center");
        right->setID("right");
        bottomLeft->setID("bottom-left");
        bottom->setID("bottom");
        bottomRight->setID("bottom-right");

        if (m_fields->setOriginalRects) {
            m_fields->originalRects.push_back(topLeft->getTextureRect());
            m_fields->originalRects.push_back(top->getTextureRect());
            m_fields->originalRects.push_back(topRight->getTextureRect());
            m_fields->originalRects.push_back(left->getTextureRect());
            m_fields->originalRects.push_back(center->getTextureRect());
            m_fields->originalRects.push_back(right->getTextureRect());
            m_fields->originalRects.push_back(bottomLeft->getTextureRect());
            m_fields->originalRects.push_back(bottom->getTextureRect());
            m_fields->originalRects.push_back(bottomRight->getTextureRect());
            m_fields->setOriginalRects = false;
        }

        setRect(topLeft, 0);
        setRect(top, 1);
        setRect(topRight, 2);
        setRect(left, 3);
        setRect(center, 4);
        setRect(right, 5);
        setRect(bottomLeft, 6);
        setRect(bottom, 7);
        setRect(bottomRight, 8);

        CCSize size = this->m_obContentSize;

        float edgeHeight = top->getContentSize().height + bottom->getContentSize().height;

        float edgeWidth = left->getContentSize().width + right->getContentSize().width;

        bool isSquishedVertically = false;
        bool isSquishedHorizontally = false;

        if (edgeHeight >= size.height) {
            isSquishedVertically = true;
        }

        if (edgeWidth >= size.width) {
            isSquishedHorizontally = true;
        }

        float sizableWidth = size.width - topLeft->getContentSize().width - topRight->getContentSize().width;
        float sizableHeight = size.height - topLeft->getContentSize().height - bottomRight->getContentSize().height;

        float horizontalScale = sizableWidth / center->getContentSize().width;
        float verticalScale = sizableHeight / center->getContentSize().height;

        center->setScaleX(horizontalScale);
        center->setScaleY(verticalScale);

        float rescaledWidth = center->getContentSize().width * horizontalScale;
        float rescaledHeight = center->getContentSize().height * verticalScale;

        float leftWidth = bottomLeft->getContentSize().width;
        float bottomHeight = bottomLeft->getContentSize().height;

        bottomLeft->setAnchorPoint({0, 0});
        bottomRight->setAnchorPoint({0, 0});
        topLeft->setAnchorPoint({0, 0});
        topRight->setAnchorPoint({0, 0});
        left->setAnchorPoint({0, 0});
        right->setAnchorPoint({0, 0});
        top->setAnchorPoint({0, 0});
        bottom->setAnchorPoint({0, 0});
        center->setAnchorPoint({0, 0});

        bottomLeft->setPosition({0, 0});
        bottomRight->setPosition({leftWidth+rescaledWidth, 0});

        topLeft->setPosition({0, bottomHeight+rescaledHeight});
        topRight->setPosition({leftWidth+rescaledWidth, bottomHeight + rescaledHeight});

        left->setPosition({0, bottomHeight});
        left->setScaleY(verticalScale);

        right->setPosition({leftWidth+rescaledWidth, bottomHeight});
        right->setScaleY(verticalScale);

        bottom->setPosition({leftWidth, 0});
        bottom->setScaleX(horizontalScale);

        top->setPosition({leftWidth, bottomHeight + rescaledHeight});
        top->setScaleX(horizontalScale);

        center->setPosition({leftWidth, bottomHeight});

        left->setVisible(true);
        right->setVisible(true);
        top->setVisible(true);
        bottom->setVisible(true);
        center->setVisible(true);

        if (isSquishedVertically || isSquishedHorizontally) {
            center->setVisible(false);
        }

        if (isSquishedVertically) {
            left->setVisible(false);
            right->setVisible(false);

            fixTopSprite(top);
            fixTopSprite(topRight);
            fixTopSprite(topLeft);

            fixBottomSprite(bottom);
            fixBottomSprite(bottomRight);
            fixBottomSprite(bottomLeft);
        }

        if (isSquishedHorizontally) {
            top->setVisible(false);
            bottom->setVisible(false);

            fixLeftSprite(left);
            fixLeftSprite(topLeft);
            fixLeftSprite(bottomLeft);

            fixRightSprite(right);
            fixRightSprite(topRight);
            fixRightSprite(bottomRight);
        }
    }

    void fixTopSprite(CCSprite* spr) {
        float overlappingHeight = spr->getContentSize().height*2 - this->getContentSize().height;

        auto rect = spr->getTextureRect();
        rect.size = CCSize{rect.size.width, rect.size.height - overlappingHeight/2};
        spr->setPositionY(spr->getPositionY() + overlappingHeight/2);
        spr->setTextureRect(rect, spr->isTextureRectRotated(), rect.size);
    }

    void fixBottomSprite(CCSprite* spr) {
        float overlappingHeight = spr->getContentSize().height*2 - this->getContentSize().height;

        auto rect = spr->getTextureRect();
        rect.size = CCSize{rect.size.width, rect.size.height - overlappingHeight/2};
        rect.origin = CCPoint{rect.origin.x, rect.origin.y + overlappingHeight/2};

        spr->setTextureRect(rect, spr->isTextureRectRotated(), rect.size);
    }

    void fixRightSprite(CCSprite* spr) {
        float overlappingWidth = spr->getContentSize().width*2 - this->getContentSize().width;

        auto rect = spr->getTextureRect();
        rect.size = CCSize{rect.size.width - overlappingWidth/2, rect.size.height};
        spr->setPositionX(spr->getPositionX() + overlappingWidth/2);
        spr->setTextureRect(rect, spr->isTextureRectRotated(), rect.size);
    }

    void fixLeftSprite(CCSprite* spr) {
        float overlappingWidth = spr->getContentSize().width*2 - this->getContentSize().width;

        auto rect = spr->getTextureRect();
        rect.size = CCSize{rect.size.width - overlappingWidth/2, rect.size.height};
        rect.origin = CCPoint{rect.origin.x + overlappingWidth/2, rect.origin.y};

        spr->setTextureRect(rect, spr->isTextureRectRotated(), rect.size);
    }
};