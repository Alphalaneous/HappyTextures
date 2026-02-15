#include <Geode/Geode.hpp>
#include <Geode/modify/TextArea.hpp>
#include <rift.hpp>
#include "TextArea.hpp"
#include "../Macros.hpp"
#include "../LabelValues.hpp"

void MyTextArea::onModify(auto& self) {
    HOOK_LATEST("TextArea::setString");
}

std::string MyTextArea::riftString(std::string_view input) {

    auto fields = m_fields.self();

    if (fields->m_isHappyTexturesModified) {
        if (!fields->m_compiled) {
            auto compiled = rift::compile(input);
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
    return std::string(input);
}

void MyTextArea::setString(gd::string p0) {
    auto fields = m_fields.self();
    if (fields->m_string != p0) {
        fields->m_compiled = false;
    }
    fields->m_string = p0;
    TextArea::setString(riftString(p0).c_str());
}

void MyTextArea::setHappyTexturesModified(bool refresh) {
    m_fields->m_isHappyTexturesModified = true;
    if (refresh) {
        TextArea::setString(m_fields->m_string);
    }
}
