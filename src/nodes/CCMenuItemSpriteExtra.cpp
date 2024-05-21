#include "CCMenuItemSpriteExtra.h"

void EventCCMenuItemSpriteExtra::setOnClick(matjson::Object onClick){
    m_fields->onClick = onClick;
    if(onClick.contains("override")){
        matjson::Value overrideVal = onClick["override"];
        if(overrideVal.is_bool()){
            m_fields->overrideOnClick = overrideVal.as_bool();
        }
    }
}
void EventCCMenuItemSpriteExtra::setOnRelease(matjson::Object onRelease){
    m_fields->onRelease = onRelease;
    if(onRelease.contains("override")){
        matjson::Value overrideVal = onRelease["override"];
        if(overrideVal.is_bool()){
            m_fields->overrideOnRelease = overrideVal.as_bool();
        }
    }
}
void EventCCMenuItemSpriteExtra::setOnActivate(matjson::Object onActivate){
    m_fields->onActivate = onActivate;
    if(onActivate.contains("override")){
        matjson::Value overrideVal = onActivate["override"];
        if(overrideVal.is_bool()){
            m_fields->overrideOnActivate = overrideVal.as_bool();
        }
    }
}
void EventCCMenuItemSpriteExtra::setOnHover(matjson::Object onHover){
    m_fields->onHover = onHover;
}
void EventCCMenuItemSpriteExtra::setOnExit(matjson::Object onExit){
    m_fields->onExit = onExit;
}

void EventCCMenuItemSpriteExtra::runOnClick(){
    UIModding::get()->handleModifications(this, m_fields->onClick);
}

void EventCCMenuItemSpriteExtra::runOnRelease(){
    UIModding::get()->handleModifications(this, m_fields->onRelease);
}

void EventCCMenuItemSpriteExtra::runOnActivate(){
    UIModding::get()->handleModifications(this, m_fields->onActivate);
}

void EventCCMenuItemSpriteExtra::runOnHover(){
    UIModding::get()->handleModifications(this, m_fields->onHover);
}

void EventCCMenuItemSpriteExtra::runOnExit(){
    UIModding::get()->handleModifications(this, m_fields->onExit);
}

void EventCCMenuItemSpriteExtra::selected(){
    if(!m_fields->overrideOnClick){
        CCMenuItemSpriteExtra::selected();
    }
    runOnClick();
}

void EventCCMenuItemSpriteExtra::unselected(){
    if(!m_fields->overrideOnRelease){
        CCMenuItemSpriteExtra::unselected();
    }
    runOnRelease();
}

void EventCCMenuItemSpriteExtra::activate(){
    if(!m_fields->overrideOnActivate){
        CCMenuItemSpriteExtra::activate();
    }
    runOnActivate();
}

CCMenuItemSpriteExtra* EventCCMenuItemSpriteExtra::create(cocos2d::CCNode* p0, cocos2d::CCNode* p1, cocos2d::CCObject* p2, cocos2d::SEL_MenuHandler p3){
    auto ret = CCMenuItemSpriteExtra::create(p0, p1, p2, p3);
    ret->schedule(schedule_selector(EventCCMenuItemSpriteExtra::checkTouch));
    return ret;
}



void EventCCMenuItemSpriteExtra::checkTouch(float dt){

    CCPoint point = getMousePos();

    CCMenu* parentMenu = static_cast<CCMenu*>(getParent());
    
    CCPoint local = convertToNodeSpace(point);
    CCRect r = rect();
    r.origin = CCPointZero;

    bool containsPoint = r.containsPoint(local);

    CCScene* currentScene = CCDirector::get()->getRunningScene();
    CCNode* buttonLayer;

    for(CCNode* node : CCArrayExt<CCNode*>(currentScene->getChildren())){
        if(node->getChildrenCount() > 1){
            buttonLayer = node;
            break;
        }
    }

    bool hasLayerOnTop = false;
    bool doCheck = false;
    for(CCNode* node : CCArrayExt<CCNode*>(currentScene->getChildren())){
        if(node == buttonLayer) {
            doCheck = true;
            continue;
        }
        if(doCheck){
            if(node->getContentSize() != CCSize{0,0} && node->isVisible()){
                hasLayerOnTop = true;
                break;
            }
        }
    }

    if(getChildOfType<GJDropDownLayer>(buttonLayer, 0)){
        hasLayerOnTop = true;
    }

    if(!hasLayerOnTop){
        if (containsPoint && !m_fields->isHovering && nodeIsVisible(this)) {
            m_fields->isHovering = true;
            runOnHover();
        }
        if (!containsPoint && m_fields->isHovering){
            m_fields->isHovering = false;
            runOnExit();
        }
    }
    else{
        m_fields->isHovering = false;
        runOnExit();
    }
}