#pragma once

#ifndef MYCCLABELBMFONT_H
#define MYCCLABELBMFONT_H

#include <Geode/Geode.hpp>
#include <Geode/modify/CCLabelBMFont.hpp>
#include "Utils.h"

using namespace geode::prelude;

class $modify(MyCCLabelBMFont, CCLabelBMFont){

    struct Fields {
        float m_limitWidth = 1;
        float m_limitDefaultScale = 1;
        float m_limitMinScale = 1;
        bool m_isLimited = false;
    };
    
    void limitLabelWidth(float width, float defaultScale, float minScale){

        m_fields->m_limitWidth = width;
        m_fields->m_limitDefaultScale = defaultScale;
        m_fields->m_limitMinScale = minScale;
        m_fields->m_isLimited = true;

        CCLabelBMFont::limitLabelWidth(width, defaultScale, minScale);
    }
};

#endif