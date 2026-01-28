#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class CustomAlert : public geode::Popup {

protected:
    bool init(float width, float height, std::string texture, std::string title);

public:
    static CustomAlert* create(float width, float height, std::string texture, std::string title);
};