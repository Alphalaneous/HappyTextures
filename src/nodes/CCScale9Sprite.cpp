#include "CCScale9Sprite.hpp"

void MyCCScale9Sprite::onModify(auto& self) {
    HOOK_LATEST("cocos2d::CCScale9Sprite::initWithFile");
    HOOK_LATEST("cocos2d::CCScale9Sprite::visit");
}

bool MyCCScale9Sprite::initWithFile(const char* file, CCRect rect, CCRect capInsets) {
    if(!CCScale9Sprite::initWithFile(file, rect, capInsets)) return false;
    m_fields->textureName = std::string(file);
    m_fields->rect = rect;
    m_fields->capInsets = capInsets;

    return true;
}

void MyCCScale9Sprite::visit() {
    bool doFix = Mod::get()->getSettingValue<bool>("ccscale9sprite-fix");

    if (doFix) {
        if (m_positionsAreDirty) {
            updateSprites();
            m_positionsAreDirty = false;
        }
        CCNode::visit();
    }
    else {
        CCScale9Sprite::visit();
    }
}

void MyCCScale9Sprite::updateSprites() {

    if (!_topLeft || !_top || !_topRight || !_left || !_centre || !_right || !_bottomLeft || !_bottom || !_bottomRight) return;

    _topLeft->setID("top-left");
    _top->setID("top");
    _topRight->setID("top-right");
    _left->setID("left");
    _centre->setID("center");
    _right->setID("right");
    _bottomLeft->setID("bottom-left");
    _bottom->setID("bottom");
    _bottomRight->setID("bottom-right");

    if (m_fields->setOriginalRects) {
        m_fields->originalRects.push_back(_topLeft->getTextureRect());
        m_fields->originalRects.push_back(_top->getTextureRect());
        m_fields->originalRects.push_back(_topRight->getTextureRect());
        m_fields->originalRects.push_back(_left->getTextureRect());
        m_fields->originalRects.push_back(_centre->getTextureRect());
        m_fields->originalRects.push_back(_right->getTextureRect());
        m_fields->originalRects.push_back(_bottomLeft->getTextureRect());
        m_fields->originalRects.push_back(_bottom->getTextureRect());
        m_fields->originalRects.push_back(_bottomRight->getTextureRect());
        m_fields->setOriginalRects = false;
    }

    setRect(_topLeft, 0);
    setRect(_top, 1);
    setRect(_topRight, 2);
    setRect(_left, 3);
    setRect(_centre, 4);
    setRect(_right, 5);
    setRect(_bottomLeft, 6);
    setRect(_bottom, 7);
    setRect(_bottomRight, 8);

    CCSize size = this->m_obContentSize;

    float edgeHeight = _top->getContentSize().height + _bottom->getContentSize().height;

    float edgeWidth = _left->getContentSize().width + _right->getContentSize().width;

    bool isSquishedVertically = false;
    bool isSquishedHorizontally = false;

    if (edgeHeight >= size.height) {
        isSquishedVertically = true;
    }

    if (edgeWidth >= size.width) {
        isSquishedHorizontally = true;
    }

    float sizableWidth = size.width - _topLeft->getContentSize().width - _topRight->getContentSize().width;
    float sizableHeight = size.height - _topLeft->getContentSize().height - _bottomRight->getContentSize().height;

    float horizontalScale = sizableWidth / _centre->getContentSize().width;
    float verticalScale = sizableHeight / _centre->getContentSize().height;

    _centre->setScaleX(horizontalScale);
    _centre->setScaleY(verticalScale);

    float rescaledWidth = _centre->getContentSize().width * horizontalScale;
    float rescaledHeight = _centre->getContentSize().height * verticalScale;

    float leftWidth = _bottomLeft->getContentSize().width;
    float bottomHeight = _bottomLeft->getContentSize().height;

    _bottomLeft->setAnchorPoint({0, 0});
    _bottomRight->setAnchorPoint({0, 0});
    _topLeft->setAnchorPoint({0, 0});
    _topRight->setAnchorPoint({0, 0});
    _left->setAnchorPoint({0, 0});
    _right->setAnchorPoint({0, 0});
    _top->setAnchorPoint({0, 0});
    _bottom->setAnchorPoint({0, 0});
    _centre->setAnchorPoint({0, 0});

    _bottomLeft->setPosition({0, 0});
    _bottomRight->setPosition({leftWidth+rescaledWidth, 0});

    _topLeft->setPosition({0, bottomHeight+rescaledHeight});
    _topRight->setPosition({leftWidth+rescaledWidth, bottomHeight + rescaledHeight});

    _left->setPosition({0, bottomHeight});
    _left->setScaleY(verticalScale);

    _right->setPosition({leftWidth+rescaledWidth, bottomHeight});
    _right->setScaleY(verticalScale);

    _bottom->setPosition({leftWidth, 0});
    _bottom->setScaleX(horizontalScale);

    _top->setPosition({leftWidth, bottomHeight + rescaledHeight});
    _top->setScaleX(horizontalScale);

    _centre->setPosition({leftWidth, bottomHeight});

    _left->setVisible(true);
    _right->setVisible(true);
    _top->setVisible(true);
    _bottom->setVisible(true);
    _centre->setVisible(true);

    if (isSquishedVertically || isSquishedHorizontally) {
        _centre->setVisible(false);
    }

    if (isSquishedVertically) {
        _left->setVisible(false);
        _right->setVisible(false);

        fixTopSprite(_top);
        fixTopSprite(_topRight);
        fixTopSprite(_topLeft);

        fixBottomSprite(_bottom);
        fixBottomSprite(_bottomRight);
        fixBottomSprite(_bottomLeft);
    }

    if (isSquishedHorizontally) {
        _top->setVisible(false);
        _bottom->setVisible(false);

        fixLeftSprite(_left);
        fixLeftSprite(_topLeft);
        fixLeftSprite(_bottomLeft);

        fixRightSprite(_right);
        fixRightSprite(_topRight);
        fixRightSprite(_bottomRight);
    }
}

void MyCCScale9Sprite::fixTopSprite(CCSprite* spr) {
    float overlappingHeight = spr->getContentSize().height*2 - this->getContentSize().height;

    auto rect = spr->getTextureRect();
    rect.size = CCSize{rect.size.width, rect.size.height - overlappingHeight/2};
    spr->setPositionY(spr->getPositionY() + overlappingHeight/2);
    spr->setTextureRect(rect, spr->isTextureRectRotated(), rect.size);
}

void MyCCScale9Sprite::fixBottomSprite(CCSprite* spr) {
    float overlappingHeight = spr->getContentSize().height*2 - this->getContentSize().height;

    auto rect = spr->getTextureRect();
    rect.size = CCSize{rect.size.width, rect.size.height - overlappingHeight/2};
    rect.origin = CCPoint{rect.origin.x, rect.origin.y + overlappingHeight/2};

    spr->setTextureRect(rect, spr->isTextureRectRotated(), rect.size);
}

void MyCCScale9Sprite::fixRightSprite(CCSprite* spr) {
    float overlappingWidth = spr->getContentSize().width*2 - this->getContentSize().width;

    auto rect = spr->getTextureRect();
    rect.size = CCSize{rect.size.width - overlappingWidth/2, rect.size.height};
    spr->setPositionX(spr->getPositionX() + overlappingWidth/2);
    spr->setTextureRect(rect, spr->isTextureRectRotated(), rect.size);
}

void MyCCScale9Sprite::fixLeftSprite(CCSprite* spr) {
    float overlappingWidth = spr->getContentSize().width*2 - this->getContentSize().width;

    auto rect = spr->getTextureRect();
    rect.size = CCSize{rect.size.width - overlappingWidth/2, rect.size.height};
    rect.origin = CCPoint{rect.origin.x + overlappingWidth/2, rect.origin.y};

    spr->setTextureRect(rect, spr->isTextureRectRotated(), rect.size);
}