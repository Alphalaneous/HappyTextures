#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCNode.hpp>
#include "../UIModding.h"
#include "../Utils.h"

using namespace geode::prelude;


class $modify(MyCCNode, CCNode) {

    struct Fields {
        matjson::Value m_lockedAttributes;
        bool m_locked;
    };

    void cleanup() {
        CCNode::cleanup();
        if (m_fields->m_locked) {
            schedule(schedule_selector(MyCCNode::lockAttributes));
        }
    }


    void setAttributesLocked(matjson::Value lockedAttributes) {
        if (!m_fields->m_locked) {
            m_fields->m_lockedAttributes = lockedAttributes;
            m_fields->m_locked = true;

            schedule(schedule_selector(MyCCNode::lockAttributes));
        }
    }


    void lockAttributes(float dt) {
        UIModding::get()->handleModifications(this, m_fields->m_lockedAttributes);
    }

};
