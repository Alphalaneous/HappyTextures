#include <Geode/Geode.hpp>
#include "Macros.h"
#include "UIModding.h"

using namespace geode::prelude;


#include <Geode/modify/CCNode.hpp>
modifyNodeForType(CCNode, init, (), ());

#include <Geode/modify/CCSprite.hpp>
modifyNodeForType(CCSprite, init, (), ());
modifyNodeForType(CCSprite, initWithTexture, (CCTexture2D *p0, const CCRect& p1, bool p2), (p0, p1, p2));
modifyNodeForType(CCSprite, initWithSpriteFrame, (CCSpriteFrame *p0), (p0));
modifyNodeForType(CCSprite, initWithSpriteFrameName, (const char *p0), (p0));
modifyNodeForType(CCSprite, initWithFile, (const char *p0, const CCRect& p1), (p0, p1));

#include <Geode/modify/CCMenu.hpp>
modifyNodeForTypeCreate(CCMenu, create, (), ());
modifyNodeForTypeCreate(CCMenu, createWithArray, (CCArray* p0), (p0));

#include <Geode/modify/CCMenuItemSpriteExtra.hpp>
modifyNodeForTypeCreate(CCMenuItemSpriteExtra, create, (cocos2d::CCNode* p0, cocos2d::CCNode* p1, cocos2d::CCObject* p2, cocos2d::SEL_MenuHandler p3), (p0, p1, p2, p3));
modifyNodeForTypeCreate(CCMenuItemSpriteExtra, create, (cocos2d::CCNode* p0, cocos2d::CCObject* p1, cocos2d::SEL_MenuHandler p2), (p0, p1, p2));

#include <Geode/modify/CCLabelBMFont.hpp>
modifyNodeForType(CCLabelBMFont, init, (), ());
modifyNodeForType(CCLabelBMFont, initWithString, (const char *p0, const char *p1, float p2, CCTextAlignment p3, CCPoint p4), (p0, p1, p2, p3, p4));

#include <Geode/modify/CCLayer.hpp>
modifyNodeForType(CCLayer, init, (), ());

#include <Geode/modify/CCScale9Sprite.hpp>
modifyNodeForType(CCScale9Sprite, init, (), ());
modifyNodeForType(CCScale9Sprite, initWithBatchNode, (CCSpriteBatchNode* p0, CCRect p1, bool p2, CCRect p3), (p0, p1, p2, p3));
modifyNodeForType(CCScale9Sprite, initWithFile, (const char* p0, CCRect p1,  CCRect p2), (p0, p1, p2));
modifyNodeForType(CCScale9Sprite, initWithSpriteFrame, (CCSpriteFrame* p0, CCRect p1), (p0, p1));
modifyNodeForType(CCScale9Sprite, initWithSpriteFrameName, (const char* p0, CCRect p1), (p0, p1));

#include <Geode/modify/CCSpriteBatchNode.hpp>
modifyNodeForType(CCSpriteBatchNode, init, (), ());
modifyNodeForType(CCSpriteBatchNode, initWithTexture, (CCTexture2D *p0, unsigned int p1), (p0, p1));
modifyNodeForType(CCSpriteBatchNode, initWithFile, (const char* p0, unsigned int p1), (p0, p1));

#include <Geode/modify/CCTextInputNode.hpp>
modifyNodeForTypeCreate(CCTextInputNode, create, (float p0, float p1, char const* p2, char const* p3, int p4, char const* p5), (p0, p1, p2, p3, p4, p5));

#include <Geode/modify/TextArea.hpp>
modifyNodeForType(TextArea, init, (gd::string p0, char const* p1, float p2, float p3, cocos2d::CCPoint p4, float p5, bool p6), (p0, p1, p2, p3, p4, p5, p6));

#include <Geode/modify/Slider.hpp>
modifyNodeForType(Slider, init, (cocos2d::CCNode* p0, cocos2d::SEL_MenuHandler p1, char const* p2, char const* p3, char const* p4, char const* p5, float p6), (p0, p1, p2, p3, p4, p5, p6));

#include <Geode/modify/ButtonSprite.hpp>
modifyNodeForType(ButtonSprite, init, (char const* p0, int p1, int p2, float p3, bool p4, char const* p5, char const* p6, float p7), (p0, p1, p2, p3, p4, p5, p6, p7));
modifyNodeForType(ButtonSprite, init, (cocos2d::CCSprite* p0, int p1, int p2, float p3, float p4, bool p5, char const* p6, bool p7), (p0, p1, p2, p3, p4, p5, p6, p7));

#include <Geode/modify/SearchButton.hpp>
modifyNodeForTypeCreate(SearchButton, create, (char const* p0, char const* p1, float p2, char const* p3), (p0, p1, p2, p3));

#include <Geode/modify/LoadingCircle.hpp>
modifyNodeForType(LoadingCircle, init, (), ());

//#include <Geode/modify/BoomScrollLayer.hpp>
//modifyNodeForTypeCreate(BoomScrollLayer, create, (cocos2d::CCArray* p0, int p1, bool p2, cocos2d::CCArray* p3, DynamicScrollDelegate* p4), (p0, p1, p2, p3, p4));

#include <Geode/modify/ExtendedLayer.hpp>
modifyNodeForType(ExtendedLayer, init, (), ());

#include <Geode/modify/SimplePlayer.hpp>
modifyNodeForType(SimplePlayer, init, (int p0), (p0));

#include <Geode/modify/DailyLevelNode.hpp>
modifyNodeForType(DailyLevelNode, init, (GJGameLevel* p0, DailyLevelPage* p1, bool p2), (p0, p1, p2));

#include <Geode/modify/GJListLayer.hpp>
modifyNodeForTypeCreate(GJListLayer, create, (BoomListView* p0, char const* p1, cocos2d::ccColor4B p2, float p3, float p4, int p5), (p0, p1, p2, p3, p4, p5));

#include <Geode/modify/CCLayerColor.hpp>
modifyNodeForType(CCLayerColor, init, (), ());
modifyNodeForType(CCLayerColor, initWithColor, (const ccColor4B& p0, GLfloat p1, GLfloat p2), (p0, p1, p2));

#include <Geode/modify/LevelCell.hpp>
modifyNodeForType(LevelCell, init, (), ());

#include <Geode/modify/CustomSongWidget.hpp>
modifyNodeForType(CustomSongWidget, init, (SongInfoObject* p0, CustomSongDelegate* p1, bool p2, bool p3, bool p4, bool p5, bool p6, bool p7), (p0, p1, p2, p3, p4, p5, p6, p7));

#include <Geode/modify/GJDifficultySprite.hpp>
modifyNodeForType(GJDifficultySprite, init, (int p0, GJDifficultyName p1), (p0, p1));

#include <Geode/modify/GJCommentListLayer.hpp>
modifyNodeForTypeCreate(GJCommentListLayer, create, (BoomListView* p0, char const* p1, cocos2d::ccColor4B p2, float p3, float p4, bool p5), (p0, p1, p2, p3, p4, p5));

