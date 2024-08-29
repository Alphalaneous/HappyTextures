#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/GJCommentListLayer.hpp>
#include "CCScale9Sprite.h"

using namespace geode::prelude;

class $modify(MyGJCommentListLayer, GJCommentListLayer) {

    struct Fields {
        SEL_SCHEDULE schedule;
        bool hasBorder = false;
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

        #ifndef GEODE_IS_MACOS

        bool doFix = Mod::get()->getSettingValue<bool>("comment-border-fix");

        if(doFix && !Loader::get()->isModLoaded("bitz.customprofiles") && !Loader::get()->isModLoaded("thesillydoggo.gradientpages")){
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
        #endif
        return ret;
    }

    #ifndef GEODE_IS_MACOS

    void listenForPosition(float dt){
        if(m_fields->hasBorder && m_fields->lastPos != getPosition()){
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
    #endif
};