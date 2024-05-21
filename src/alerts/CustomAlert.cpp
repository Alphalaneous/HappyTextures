#include <Geode/Geode.hpp>
#include "CustomAlert.h"

CustomAlert* CustomAlert::create(float width, float height, std::string texture, std::string title) {
    auto pRet = new CustomAlert();
    if (pRet && pRet->init(width, height, texture.c_str(), title.c_str())) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
};
