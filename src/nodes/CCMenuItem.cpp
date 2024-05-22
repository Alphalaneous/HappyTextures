#include "CCMenuItem.h"

void EventCCMenuItem::setOnClick(matjson::Object onClick){
    m_fields->onClick = onClick;
    if(onClick.contains("override")){
        matjson::Value overrideVal = onClick["override"];
        if(overrideVal.is_bool()){
            m_fields->overrideOnClick = overrideVal.as_bool();
        }
    }
}
void EventCCMenuItem::setOnRelease(matjson::Object onRelease){
    m_fields->onRelease = onRelease;
    if(onRelease.contains("override")){
        matjson::Value overrideVal = onRelease["override"];
        if(overrideVal.is_bool()){
            m_fields->overrideOnRelease = overrideVal.as_bool();
        }
    }
}
void EventCCMenuItem::setOnActivate(matjson::Object onActivate){
    m_fields->onActivate = onActivate;
    if(onActivate.contains("override")){
        matjson::Value overrideVal = onActivate["override"];
        if(overrideVal.is_bool()){
            m_fields->overrideOnActivate = overrideVal.as_bool();
        }
    }
}
void EventCCMenuItem::setOnHover(matjson::Object onHover){
    m_fields->hasHover = true;
    m_fields->onHover = onHover;
}
void EventCCMenuItem::setOnExit(matjson::Object onExit){
    m_fields->hasExit = true;
    m_fields->onExit = onExit;
}

void EventCCMenuItem::runOnClick(){
    UIModding::get()->handleModifications(this, m_fields->onClick);
}

void EventCCMenuItem::runOnRelease(){
    UIModding::get()->handleModifications(this, m_fields->onRelease);
}

void EventCCMenuItem::runOnActivate(){
    UIModding::get()->handleModifications(this, m_fields->onActivate);
}

void EventCCMenuItem::runOnHover(){
    UIModding::get()->handleModifications(this, m_fields->onHover);
}

void EventCCMenuItem::runOnExit(){
    UIModding::get()->handleModifications(this, m_fields->onExit);
}

void EventCCMenuItem::selected(){
    if(!m_fields->overrideOnClick){
        CCMenuItem::selected();
    }
    runOnClick();
}

void EventCCMenuItem::unselected(){
    if(!m_fields->overrideOnRelease){
        CCMenuItem::unselected();
    }
    runOnRelease();
}

void EventCCMenuItem::activate(){
    if(!m_fields->overrideOnActivate){
        CCMenuItem::activate();
    }
    runOnActivate();
}


void EventCCMenuItem::checkTouch(bool hasLayerOnTop){

    if((m_fields->hasHover || m_fields->hasExit) && nodeIsVisible(this)){

        CCPoint point = getMousePos();

        CCMenu* parentMenu = static_cast<CCMenu*>(getParent());
    
        CCPoint local = convertToNodeSpace(point);
        CCRect r = rect();
        r.origin = CCPointZero;

        CCSize screenSize = CCDirector::get()->getWinSize();

        CCRect screenRect = CCRect{0, 0, screenSize.width, screenSize.height};

        bool containsPoint = r.containsPoint(local);
       
        if(!hasLayerOnTop){
            if (containsPoint && !m_fields->isHovering) {
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
}