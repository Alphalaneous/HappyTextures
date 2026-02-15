#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCNode.hpp>
#include "../Macros.hpp"
#include "../UIModding.hpp"

using namespace geode::prelude;

class $modify(MyCCNode, CCNode) {

    struct Fields {
        std::vector<matjson::Value> m_attributes;
        std::vector<CCAction*> m_scheduledAttributes;
        bool m_modified = false;
        CCNode* m_self;
        std::string m_baseID = "";
        ~Fields() {
            UIModding::get()->m_nodeVTables.erase(m_self);
        }
    };

    void setBaseID(std::string_view ID);

    std::string_view getBaseID();

    void setAttributes(const matjson::Value& attributes);

    void clearAttributes();

    void scheduleAttribute(const matjson::Value& attributes);

    void resetScheduledAttributes();

    std::vector<matjson::Value> getAttributes();

    bool isModified();

    void setModified();
};
