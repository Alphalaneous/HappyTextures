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
        std::string m_baseID = "";
    };

    void setBaseID(const std::string& ID) {
        m_fields->m_baseID = ID;
    }

    std::string getBaseID() {
        return m_fields->m_baseID;
    }

    void setAttributes(const matjson::Value& attributes) {
        m_fields->m_attributes.push_back(attributes);
    }

    void clearAttributes() {
        m_fields->m_attributes.clear();
    }

    void scheduleAttribute(const matjson::Value& attributes) {
        auto fields = m_fields.self();
        
        CCActionInstant* callFunc = CallFuncExt::create([this, attributes] {
            UIModding::get()->handleModifications(this, attributes);
        });
        CCDelayTime* delay = CCDelayTime::create(attributes["delay"].asDouble().unwrapOr(0));
        CCSequence* sequence = CCSequence::create(callFunc, delay, nullptr);
        CCRepeatForever* repeat = CCRepeatForever::create(sequence);

        fields->m_scheduledAttributes.push_back(repeat);

        runAction(repeat);
    }

    void resetScheduledAttributes() {
        auto fields = m_fields.self();

        for (CCAction* action : fields->m_scheduledAttributes) {
            stopAction(action);
        }
    }

    std::vector<matjson::Value> getAttributes() {
        return m_fields->m_attributes;
    }

    bool isModified() {
        return m_fields->m_modified;
    }

    void setModified() {
        m_fields->m_modified = true;
    }
};
