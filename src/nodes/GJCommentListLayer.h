#pragma once

#ifndef MYGJCOMMENTLISTLAYER_H
#define MYGJCOMMENTLISTLAYER_H

#include <Geode/Geode.hpp>
#include <Geode/modify/GJCommentListLayer.hpp>
#include "CCScale9Sprite.h"

using namespace geode::prelude;

class $modify(MyGJCommentListLayer, GJCommentListLayer) {

	struct Fields {
		SEL_SCHEDULE schedule;
		CCSize size;
		CCNode* border;
		CCPoint lastPos;
	};

    static void onModify(auto& self) {
        (void) self.setHookPriority("GJCommentListLayer::create", INT_MIN);
    }

	static GJCommentListLayer* create(BoomListView* p0, char const* p1, cocos2d::ccColor4B p2, float p3, float p4, bool p5) {
		auto ret = GJCommentListLayer::create(p0, p1, p2, p3, p4, p5);
		if(UIModding::get()->doModify){
			if(ret->getColor() == ccColor3B{191,114,62}){
				std::optional<ColorData> dataOpt = UIModding::get()->getColors("comment-list-layer-bg");
				if(dataOpt.has_value()){
					ColorData data = dataOpt.value();
					ret->setColor(data.color);
					ret->setOpacity(data.alpha);
				}
			}
		}
		bool doFix = Mod::get()->getSettingValue<bool>("comment-border-fix");

        if(doFix && !Loader::get()->isModLoaded("bitz.customprofiles")){
			if(CCNode* node = ret->getChildByID("left-border")) {
				node->setVisible(false);
			}
			if(CCNode* node = ret->getChildByID("right-border")) {
				node->setVisible(false);
			}
			if(CCNode* node = ret->getChildByID("top-border")) {
				node->setVisible(false);
			}
			if(CCNode* node = ret->getChildByID("bottom-border")) {
				node->setVisible(false);
			}
		
			MyGJCommentListLayer* myRet = static_cast<MyGJCommentListLayer*>(ret);
			myRet->m_fields->schedule = schedule_selector(MyGJCommentListLayer::checkForParent);
			myRet->m_fields->size = CCSize{p3, p4};
			myRet->schedule(myRet->m_fields->schedule);
			myRet->schedule(schedule_selector(MyGJCommentListLayer::listenForPosition));
	
			CCPoint pos = {p3/2, p4/2};

			CCScale9Sprite* outlineSprite = CCScale9Sprite::create("commentBorder.png"_spr);
			outlineSprite->setContentSize({p3 + 1.6f, p4 + 1.6f});
			outlineSprite->setPosition({pos.x, pos.y});
			outlineSprite->setZOrder(20);
			outlineSprite->setID("outline");

			if(!p5){
				outlineSprite->setColor({130, 64, 32});
				std::optional<ColorData> dataOpt = UIModding::get()->getColors("comment-list-outline-brown");
				if(dataOpt.has_value()){
					ColorData data = dataOpt.value();
					outlineSprite->setColor(data.color);
				}
			}
			else{
				outlineSprite->setColor({32, 49, 130});
				std::optional<ColorData> dataOpt = UIModding::get()->getColors("comment-list-outline-blue");
				if(dataOpt.has_value()){
					ColorData data = dataOpt.value();
					outlineSprite->setColor(data.color);
				}
			}
			
			myRet->addChild(outlineSprite);
		}
		return ret;
	}

	void listenForPosition(float dt){
		if(m_fields->border && m_fields->lastPos != getPosition()){
			if(CCNode* parent = getParent()){
				updateBordersWithParent(parent);
			}
			m_fields->lastPos = getPosition();
		}
	}

	void checkForParent(float dt){
		if(CCNode* parent = getParent()){
			updateBordersWithParent(parent);
			unschedule(m_fields->schedule);
		}
	}

	void updateBordersWithParent(CCNode* parent){

		if(CCScale9Sprite* bg = typeinfo_cast<CCScale9Sprite*>(parent->getChildByID("background"))){
			createMask(bg);
		}
		else if(CCScale9Sprite* bg = getChildOfType<CCScale9Sprite>(parent, 0)){
			createMask(bg);
		}
	}

	void createMask(CCScale9Sprite* bg){

		removeChildByID("special-border");

		CCNode* parent = CCNode::create();

		CCNode* mask = CCNode::create();

		CCPoint pos = {m_fields->size.width/2, m_fields->size.height/2};
		
		CCScale9Sprite* innerCurve = CCScale9Sprite::create("square02b_001.png");
		innerCurve->setContentSize(m_fields->size);
		innerCurve->setPosition(pos);

		mask->addChild(innerCurve);
		mask->setContentSize(m_fields->size);

		CCSprite* clipTop = CCSprite::createWithSpriteFrameName("block005b_05_001.png");
		clipTop->setPosition({m_fields->size.width/2, m_fields->size.height + 5});
		clipTop->setScaleX(50);
		clipTop->setScaleY(50);
		clipTop->setAnchorPoint({0.5, 0});
		mask->addChild(clipTop);

		CCSprite* clipBottom = CCSprite::createWithSpriteFrameName("block005b_05_001.png");
		clipBottom->setPosition({m_fields->size.width/2, -5});
		clipBottom->setScaleX(50);
		clipBottom->setScaleY(50);
		clipBottom->setAnchorPoint({0.5, 1});
		mask->addChild(clipBottom);

		CCSprite* clipLeft = CCSprite::createWithSpriteFrameName("block005b_05_001.png");
		clipLeft->setPosition({-5, m_fields->size.height/2});
		clipLeft->setScaleX(50);
		clipLeft->setScaleY(50);
		clipLeft->setAnchorPoint({1, 0.5});
		mask->addChild(clipLeft);

		CCSprite* clipRight = CCSprite::createWithSpriteFrameName("block005b_05_001.png");
		clipRight->setPosition({m_fields->size.width + 5, m_fields->size.height/2});
		clipRight->setScaleX(50);
		clipRight->setScaleY(50);
		clipRight->setAnchorPoint({0, 0.5});
		mask->addChild(clipRight);

		m_fields->border = mask;
		mask->setZOrder(20);

		CCSize winSize = CCDirector::get()->getWinSize();

		CCRenderTexture* renderStencil = CCRenderTexture::create(winSize.width, winSize.height);

		CCPoint maskPos = getPosition();
		mask->setPosition(maskPos);
		parent->addChild(mask);

		renderStencil->setPosition(-maskPos.x, -maskPos.y);
		renderStencil->setContentSize(winSize);
		renderStencil->setAnchorPoint({0,0});
		renderStencil->begin();
		parent->visit();
		renderStencil->end();
		CCSprite* spr = typeinfo_cast<CCSprite*>(renderStencil->getChildren()->objectAtIndex(0));
		spr->ignoreAnchorPointForPosition(true);
		spr->removeFromParent();

		MyCCScale9Sprite* myBG = static_cast<MyCCScale9Sprite*>(bg);

		CCScale9Sprite* newBG = CCScale9Sprite::create(myBG->m_fields->textureName, myBG->m_fields->rect, myBG->m_fields->capInsets);
		newBG->setContentSize(myBG->getContentSize());
		newBG->ignoreAnchorPointForPosition(true);
		CCRenderTexture* renderBG = CCRenderTexture::create(newBG->getContentSize().width, newBG->getContentSize().height);
		renderBG->begin();
		newBG->visit();
		renderBG->end();
		
		CCSprite* bgSpr = typeinfo_cast<CCSprite*>(renderBG->getChildren()->objectAtIndex(0));
		bgSpr->setPosition(bg->getPosition());
		bgSpr->setContentSize(newBG->getContentSize());
		bgSpr->removeFromParent();

		CCNode* parentNode = CCNode::create();

		CCClippingNode* clippingNode = CCClippingNode::create();
		clippingNode->setInverted(true);
		clippingNode->addChild(bgSpr);
		clippingNode->setStencil(spr);
		clippingNode->setContentSize(winSize);
		clippingNode->setPosition(-maskPos.x + 5, -maskPos.y + 5);
		clippingNode->setAnchorPoint({0, 0});
		clippingNode->setAlphaThreshold(0.7f);

		parentNode->setAnchorPoint({0, 0});
		parentNode->setContentSize({getContentSize().width+10, getContentSize().height+10});
		parentNode->setPosition({-5, -5});
		parentNode->setID("special-border");
		parentNode->setZOrder(19);
		parentNode->addChild(clippingNode);

		addChild(parentNode);
	}
};

#endif