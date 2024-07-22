#include <Geode/Geode.hpp>
#include "Macros.h"
#include "UIModding.h"

using namespace geode::prelude;

#include <Geode/modify/CCSprite.hpp>
modifyNodeForTypeCreate(CCSprite, create, (), ());

#ifndef GEODE_IS_MACOS
modifyNodeForTypeCreate(CCSprite, createWithTexture, (CCTexture2D *p0, const CCRect& p1), (p0, p1));
#endif

modifyNodeForTypeCreate(CCSprite, createWithSpriteFrame, (CCSpriteFrame *p0), (p0));

#ifndef GEODE_IS_MACOS
#include <Geode/modify/CCMenu.hpp>
modifyNodeForType(CCMenu, initWithArray, (CCArray* p0), (p0));
#endif

#include <Geode/modify/CCMenuItemSpriteExtra.hpp>
modifyNodeForType(CCMenuItemSpriteExtra, init, (cocos2d::CCNode* p0, cocos2d::CCNode* p1, cocos2d::CCObject* p2, cocos2d::SEL_MenuHandler p3), (p0, p1, p2, p3));

#include <Geode/modify/CCLabelBMFont.hpp>
modifyNodeForType(CCLabelBMFont, init, (), ());
modifyNodeForType(CCLabelBMFont, initWithString, (const char *p0, const char *p1, float p2, CCTextAlignment p3, CCPoint p4), (p0, p1, p2, p3, p4));

#include <Geode/modify/CCScale9Sprite.hpp>
modifyNodeForType(CCScale9Sprite, init, (), ());

#ifndef GEODE_IS_MACOS

modifyNodeForType(CCScale9Sprite, initWithBatchNode, (CCSpriteBatchNode* p0, CCRect p1, bool p2, CCRect p3), (p0, p1, p2, p3));
modifyNodeForType(CCScale9Sprite, initWithFile, (const char* p0, CCRect p1,  CCRect p2), (p0, p1, p2));
modifyNodeForType(CCScale9Sprite, initWithSpriteFrame, (CCSpriteFrame* p0, CCRect p1), (p0, p1));
modifyNodeForType(CCScale9Sprite, initWithSpriteFrameName, (const char* p0, CCRect p1), (p0, p1));

#endif

#include <Geode/modify/CCTextInputNode.hpp>
modifyNodeForTypeCreate(CCTextInputNode, create, (float p0, float p1, char const* p2, char const* p3, int p4, char const* p5), (p0, p1, p2, p3, p4, p5));

#include <Geode/modify/TextArea.hpp>
modifyNodeForTypeCreate(TextArea, create, (gd::string p0, char const* p1, float p2, float p3, cocos2d::CCPoint p4, float p5, bool p6), (p0, p1, p2, p3, p4, p5, p6));

#ifndef GEODE_IS_MACOS

#include <Geode/modify/SearchButton.hpp>
modifyNodeForTypeCreate(SearchButton, create, (char const* p0, char const* p1, float p2, char const* p3), (p0, p1, p2, p3));

#include <Geode/modify/SimplePlayer.hpp>
modifyNodeForType(SimplePlayer, init, (int p0), (p0));

#endif

#include <Geode/modify/CCLayer.hpp>
modifyNodeForType(CCLayer, init, (), ());

#include <Geode/modify/CCLayerColor.hpp>
modifyNodeForType(CCLayerColor, init, (), ());
modifyNodeForType(CCLayerColor, initWithColor, (const ccColor4B& p0, GLfloat p1, GLfloat p2), (p0, p1, p2));

#include <Geode/modify/LoadingCircle.hpp>
modifyNodeForType(LoadingCircle, init, (), ());
