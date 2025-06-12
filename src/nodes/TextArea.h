#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/TextArea.hpp>
#include <rift.hpp>
#include "../Macros.h"

class $modify(MyTextArea, TextArea) {

    static void onModify(auto& self) {
        HOOK_LATEST("TextArea::setString");
    }

    struct Fields {
        bool m_isHappyTexturesModified;
        std::string m_string;
        std::unique_ptr<rift::Script> m_riftScript;
        bool m_compiled = false;
    };

    std::string riftString(std::string input) {

        auto fields = m_fields.self();

        if (fields->m_isHappyTexturesModified) {
            if (!fields->m_compiled) {
                auto compiled = rift::compile(std::string_view(input));
                if (compiled) {
                    fields->m_riftScript = std::move(compiled.unwrap());
                }
                fields->m_compiled = true;
            }
            if (fields->m_riftScript) {
                auto vars = LabelValues::getValueMap(fields->m_string);
                auto res = fields->m_riftScript->run(vars);
                if (res) {
                    std::string newNewString = res.unwrap();
                    return newNewString;
                }
            }
        }
        return input;
    }

    void setString(gd::string p0) {
        auto fields = m_fields.self();
        if (fields->m_string != p0) {
            fields->m_compiled = false;
        }
        fields->m_string = p0;
        TextArea::setString(riftString(p0).c_str());
    }

    void setHappyTexturesModified(bool refresh = false) {
        m_fields->m_isHappyTexturesModified = true;
        if (refresh) {
            TextArea::setString(m_fields->m_string);
        }
    }

};