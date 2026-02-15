#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/TextArea.hpp>
#include <rift.hpp>

using namespace geode::prelude;

class $modify(MyTextArea, TextArea) {

    static void onModify(auto& self);

    struct Fields {
        bool m_isHappyTexturesModified;
        std::string m_string;
        std::unique_ptr<rift::Script> m_riftScript;
        bool m_compiled = false;
    };

    std::string riftString(std::string_view input);
    void setString(gd::string p0);
    void setHappyTexturesModified(bool refresh = false);
};