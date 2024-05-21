#pragma once

#ifndef __BLOCKMENU_HPP
#define __BLOCKMENU_HPP

#include "AlertDelegate.h"

class CustomAlert : public AlertDelegate {
    public:
        static CustomAlert* create(float width, float height, std::string texture, std::string title);
};

#endif