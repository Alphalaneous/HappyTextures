#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/CCLabelBMFont.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/CCSpriteBatchNode.hpp>
#include <Geode/modify/CCScale9Sprite.hpp>

#define public_cast(value, member) [](auto* v) { \
    class FriendClass__; \
    using T = std::remove_pointer<decltype(v)>::type; \
    class FriendeeClass__: public T { \
    protected: \
        friend FriendClass__; \
    }; \
    class FriendClass__ { \
    public: \
        auto& get(FriendeeClass__* v) { return v->member; } \
    } c; \
    return c.get(reinterpret_cast<FriendeeClass__*>(v)); \
}(value)

using namespace geode::prelude;

bool s_isInCreateTextLayers = false;

class $modify(GJBaseGameLayer) {
	
	void createTextLayers(){

		s_isInCreateTextLayers = true;
		GJBaseGameLayer::createTextLayers();
		s_isInCreateTextLayers = false;

	}
};

class $modify(MyCCScale9Sprite, CCScale9Sprite){

	void visit(){

		if(this->*(&MyCCScale9Sprite::m_positionsAreDirty)){
			updateSprites();
			this->*(&MyCCScale9Sprite::m_positionsAreDirty) = false;
		}
    	CCNode::visit();
	}

	void updateSprites(){

		CCSprite* topLeft = public_cast(this, _topLeft);
		topLeft->setID("top-left");
		CCSprite* top = public_cast(this, _top);
		top->setID("top");
		CCSprite* topRight = public_cast(this, _topRight);
		topRight->setID("top-right");
		CCSprite* left = public_cast(this, _left);
		left->setID("left");
		CCSprite* center = public_cast(this, _centre);
		center->setID("center");
		CCSprite* right = public_cast(this, _right);
		right->setID("right");
		CCSprite* bottomLeft = public_cast(this, _bottomLeft);
		bottomLeft->setID("bottom-left");
		CCSprite* bottom = public_cast(this, _bottom);
		bottom->setID("bottom");
		CCSprite* bottomRight = public_cast(this, _bottomRight);
		bottomRight->setID("bottom-right");

		if(!(topLeft && topRight && bottomRight && bottomLeft && center)) {
			return;
		}

		CCSize size = this->m_obContentSize;

		float edgeHeight = top->getContentSize().height + bottom->getContentSize().height;

		float edgeWidth = left->getContentSize().width + right->getContentSize().width;

		bool isSquishedVertically = false;
		bool isSquishedHorizontally = false;

		if(edgeHeight >= size.height){
			isSquishedVertically = true;
		}

		if(edgeWidth >= size.width){
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

		if(isSquishedVertically || isSquishedHorizontally){
			center->setVisible(false);
		}

		if(isSquishedVertically){
			left->setVisible(false);
			right->setVisible(false);

			fixTopSprite(top);
			fixTopSprite(topRight);
			fixTopSprite(topLeft);

			fixBottomSprite(bottom);
			fixBottomSprite(bottomRight);
			fixBottomSprite(bottomLeft);
		}

		if(isSquishedHorizontally){
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

	void fixTopSprite(CCSprite* spr){
		float overlappingHeight = spr->getContentSize().height*2 - this->getContentSize().height;
		
		auto rect = spr->getTextureRect();
		rect.size = CCSize{rect.size.width, rect.size.height - overlappingHeight/2};
		spr->setPositionY(spr->getPositionY() + overlappingHeight/2);
		spr->setTextureRect(rect);
	}

	void fixBottomSprite(CCSprite* spr){
		float overlappingHeight = spr->getContentSize().height*2 - this->getContentSize().height;
		
		auto rect = spr->getTextureRect();
		rect.size = CCSize{rect.size.width, rect.size.height - overlappingHeight/2};
		rect.origin = CCPoint{rect.origin.x, rect.origin.y + overlappingHeight/2};

		spr->setTextureRect(rect);
	}

	void fixRightSprite(CCSprite* spr){
		float overlappingWidth = spr->getContentSize().width*2 - this->getContentSize().width;
		
		auto rect = spr->getTextureRect();
		rect.size = CCSize{rect.size.width - overlappingWidth/2, rect.size.height};
		spr->setPositionX(spr->getPositionX() + overlappingWidth/2);
		spr->setTextureRect(rect);
	}

	void fixLeftSprite(CCSprite* spr){
		float overlappingWidth = spr->getContentSize().width*2 - this->getContentSize().width;
		
		auto rect = spr->getTextureRect();
		rect.size = CCSize{rect.size.width - overlappingWidth/2, rect.size.height};
		rect.origin = CCPoint{rect.origin.x + overlappingWidth/2, rect.origin.y};

		spr->setTextureRect(rect);
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
