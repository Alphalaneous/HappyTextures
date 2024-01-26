#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/CCLabelBMFont.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/CCSpriteBatchNode.hpp>

using namespace geode::prelude;

bool s_isInCreateTextLayers = false;

class $modify(GJBaseGameLayer) {
	
	void createTextLayers(){

		s_isInCreateTextLayers = true;
		GJBaseGameLayer::createTextLayers();
		s_isInCreateTextLayers = false;

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
