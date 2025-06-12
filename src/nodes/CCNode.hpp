#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCNode.hpp>
#include "../Macros.hpp"

using namespace geode::prelude;

class $modify(MyCCNode, CCNode) {

    struct Fields {
        matjson::Value m_attributes;
        bool m_modified = false;
    };

    void setAttributes(const matjson::Value& attributes) {
        m_fields->m_attributes = attributes;
    }

    matjson::Value getAttributes() {
        return m_fields->m_attributes;
    }

    bool isModified() {
        return m_fields->m_modified;
    }

    void setModified() {
        m_fields->m_modified = true;
    }
};
