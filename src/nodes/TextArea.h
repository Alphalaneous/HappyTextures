#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/TextArea.hpp>
#include <rift.hpp>
#include "CCLabelBMFont.h"

class $modify(MyTextArea, TextArea) {

    struct Fields {
        bool m_isHappyTexturesModified;
        std::string m_string;
    };

    static std::string riftString(MyTextArea* self, std::string input) {
        if (self->m_fields->m_isHappyTexturesModified) {
            rift::Result<rift::Script*> scriptRes = rift::compile(std::string_view(input));
            if (!scriptRes.isError()) {
                rift::Script* script = scriptRes.unwrap();
                auto vars = LabelValues::getValueMap(self->m_fields->m_string);
                std::string newNewString = script->run(vars);
                delete script;
                return newNewString;
            }
        }
        return input;
    }

    void setString(gd::string p0) {
        m_fields->m_string = p0;
        TextArea::setString(riftString(this, p0).c_str());
    }

    void setHappyTexturesModified(bool refresh = false) {
        m_fields->m_isHappyTexturesModified = true;
        if (refresh) {
            TextArea::setString(m_fields->m_string);
        }
    }

};