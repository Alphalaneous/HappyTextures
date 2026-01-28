#include <Geode/Geode.hpp>
#include "CustomAlert.hpp"

CustomAlert* CustomAlert::create(float width, float height, std::string texture, std::string title) {
    auto ret = new CustomAlert();
    if (ret->init(width, height, texture.c_str(), title.c_str())) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
};

bool CustomAlert::init(float width, float height, std::string texture, std::string title){
    if (!Popup::init(width, height, texture.c_str())) return false;

    setTitle(title);

    return true;
}