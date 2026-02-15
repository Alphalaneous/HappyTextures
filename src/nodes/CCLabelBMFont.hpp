#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCLabelBMFont.hpp>
#include "../Macros.hpp"
#include <rift.hpp>

using namespace geode::prelude;

class $modify(MyCCLabelBMFont, CCLabelBMFont) {

    static void onModify(auto& self);

    struct Fields {
        float m_limitWidth = 1;
        float m_limitDefaultScale = 1;
        float m_limitMinScale = 1;
        bool m_isLimited = false;
        bool m_isHappyTexturesModified;
        std::unique_ptr<rift::Script> m_riftScript;
        bool m_compiled = false;
    };
    
    std::string riftString(std::string_view input);

    void setString(const char *newString, bool needUpdateLabel);

    void setHappyTexturesModified(bool refresh = false);

    void limitLabelWidth(float width, float defaultScale, float minScale);

    void checkParent(float dt);
};