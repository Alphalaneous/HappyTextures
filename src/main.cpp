#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/CCLabelBMFont.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/CCSpriteBatchNode.hpp>
#include <Geode/modify/CCScale9Sprite.hpp>

using namespace geode::prelude;

bool s_isInCreateTextLayers = false;

class $modify(GJBaseGameLayer) {
	
	void createTextLayers(){

		s_isInCreateTextLayers = true;
		GJBaseGameLayer::createTextLayers();
		s_isInCreateTextLayers = false;

	}
};

class $modify(CCScale9Sprite) {
	bool m_hasFixed = false;
	void visit() {
		CCScale9Sprite::visit();
		if (!m_fields->m_hasFixed) {
			fixSprites();
			m_fields->m_hasFixed = true;
		}
	}

	void transformSprite(CCSprite* sprite, CCSize transform, int idx) {
		auto tRect = sprite->getTextureRect();

		tRect.size -= transform;

		if (idx == 0)
			sprite->setPosition(sprite->getPosition() + transform);
		else if (idx == 1 && (transform.width > 0 || transform.height > 0))
			sprite->setVisible(false);
		else if (idx == 2)
			tRect.origin += transform;

		sprite->setTextureRect(tRect);
	}

	void fixSprites() {
		CCSprite* sprites[3][3] = {
			{_topLeft, _top, _topRight},
			{_left, _centre, _right},
			{_bottomLeft, _bottom, _bottomRight}
		};

		auto halfSize = getContentSize() / 2;

		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 3; ++j) {
				auto sprite = sprites[i][j];

				auto tRect = sprite->getTextureRect();
				auto diff = tRect.size - halfSize;
				diff = CCSize(fmax(0, diff.width), fmax(0, diff.height));

				transformSprite(sprite, CCSize(diff.width, 0), j);
				transformSprite(sprite, CCSize(0, diff.height), i);
			}
		}
	}
};

class $modify(CCSpriteBatchNode) {
	
	bool initWithTexture(CCTexture2D* texture, unsigned int capacity){

		if(s_isInCreateTextLayers && texture == CCTextureCache::sharedTextureCache()->addImage("bigFont.png", false)){
			return CCSpriteBatchNode::initWithTexture(CCTextureCache::sharedTextureCache()->addImage("bigFont.png"_spr, false), capacity);
		}

		return CCSpriteBatchNode::initWithTexture(texture, capacity);
	}
};


class $modify(CCLabelBMFont) {
	
	static CCLabelBMFont* createBatched(const char* str, const char* fntFile, CCArray* a, int a1){

		if(strcmp(fntFile, "bigFont.fnt") == 0){
			fntFile = "bigFont.fnt"_spr;
		}

		return CCLabelBMFont::createBatched(str, fntFile, a, a1);
	}
};

class $modify(GameManager) {

	const char* getFontTexture(int val){

		if(val > 58){
			val = 59;
		}

		loadFont(val);

		if(val != 0){
			CCString* str = CCString::createWithFormat("gjFont%02d.png", val);
			return str->getCString();
		}
		
		return "bigFont.png"_spr;
	}

	void loadFont(int val){

		if(val > 58){
			val = 59;
		}

		if(val != m_loadedFont){

			if(m_loadedFont != 0){
				CCTextureCache::sharedTextureCache()->removeTextureForKey(CCString::createWithFormat("gjFont%02d.png", m_loadedFont)->getCString());
			}
			if(val == 0){
				CCTextureCache::sharedTextureCache()->addImage("bigFont.png"_spr, false);
			}
			else {
				CCTextureCache::sharedTextureCache()->addImage(CCString::createWithFormat("gjFont%02d.png", val)->getCString(), false);
			}
		}
		m_loadedFont = val;
	}

};

class $modify(MenuLayer) {

	bool init(){
		if(!MenuLayer::init()){
			return false;
		}
	
		CCSprite* spr = dynamic_cast<CCSprite*>(this->getChildByID("main-title"));

		CCSprite* newSpr = CCSprite::create("GJ_title.png");

		if(newSpr){

			newSpr->setPosition(spr->getPosition());
			newSpr->setAnchorPoint(spr->getAnchorPoint());
			newSpr->setScale(spr->getScale());
			newSpr->setVisible(spr->isVisible());
			newSpr->setRotation(spr->getRotation());
			newSpr->setZOrder(spr->getZOrder());
			newSpr->ignoreAnchorPointForPosition(spr->isIgnoreAnchorPointForPosition());
			newSpr->setColor(spr->getColor());

			spr->removeFromParentAndCleanup(true);
			newSpr->setID("main-title");
			this->addChild(newSpr);
		}

		return true;
	}

};
