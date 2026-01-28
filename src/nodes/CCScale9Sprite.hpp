#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCScale9Sprite.hpp>
#include "../Macros.hpp"

using namespace geode::prelude;

#define setRect(node, idx) node->setTextureRect(m_fields->originalRects.at(idx), node->isTextureRectRotated(), m_fields->originalRects.at(idx).size);

class $modify(MyCCScale9Sprite, CCScale9Sprite) {

    static void onModify(auto& self);

    struct Fields {
        std::string textureName;
        CCRect rect;
        CCRect capInsets;
        std::vector<CCRect> originalRects;
        bool setOriginalRects = true;
    };

    bool initWithFile(const char* file, CCRect rect, CCRect capInsets);

    void visit();
    void updateSprites();
    void fixTopSprite(CCSprite* spr);

    void fixBottomSprite(CCSprite* spr);

    void fixRightSprite(CCSprite* spr);

    void fixLeftSprite(CCSprite* spr);
};