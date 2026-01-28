#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCLabelBMFont.hpp>
#include "../Macros.hpp"
#include "Geode/utils/ZStringView.hpp"

#include <rift.hpp>

using namespace geode::prelude;

class $modify(MyCCLabelBMFont, CCLabelBMFont) {

    static void onModify(auto& self);

    struct Fields {
        float m_limitWidth = 1;
        float m_limitDefaultScale = 1;
        float m_limitMinScale = 1;
        bool m_isLimited = false;
        SEL_SCHEDULE m_schedule;
        bool m_isHappyTexturesModified;
        std::unique_ptr<rift::Script> m_riftScript;
        bool m_compiled = false;
    };
    
    std::string riftString(ZStringView input);

    void setString(const char *newString, bool needUpdateLabel);

    void setHappyTexturesModified(bool refresh = false);

    void limitLabelWidth(float width, float defaultScale, float minScale);

    static CCLabelBMFont* create(const char *str, const char *fntFile, float width, CCTextAlignment alignment, CCPoint imageOffset);

    void checkParent(float dt);
};