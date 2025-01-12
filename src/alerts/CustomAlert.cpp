#include <Geode/Geode.hpp>
#include "CustomAlert.h"
#include "../nodes/CCLabelBMFont.h"

CustomAlert* CustomAlert::create(float width, float height, std::string texture, std::string title) {
    auto pRet = new CustomAlert();
    if (pRet && pRet->init(width, height, texture.c_str(), title.c_str())) {
        pRet->autorelease();
        return pRet;
    }
    delete pRet;
    return nullptr;
};

bool CustomAlert::init(float width, float height, std::string texture, std::string title){
  
    if (!Popup<>::initAnchored(width, height, texture.c_str())) return false;

    setTitle(title);

    return true;
}

bool CustomAlert::setup(){
    return true;
}