#pragma once

#include "AlertDelegate.h"

class CustomAlert : public AlertDelegate {
    public:
        static CustomAlert* create(float width, float height, std::string texture, std::string title);
};