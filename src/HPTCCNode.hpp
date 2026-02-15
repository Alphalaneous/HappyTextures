#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCNode.hpp>
#include "HPTParser.hpp"
#include "TouchObject.hpp"

using namespace geode::prelude;

struct UISchedule {
    std::shared_ptr<HPTNode> m_node;
    Ref<CCAction> m_action;

    UISchedule(std::shared_ptr<HPTNode> node, CCAction* action) {
        m_node = node;
        m_action = action;
    }

    CCAction* get() {
        return m_action;
    }
};

class $modify(HPTCCNode, CCNode) {

    struct Fields {
        TouchObject* m_touchObject;
        std::vector<std::shared_ptr<HPTNode>> m_owned;
        std::vector<std::shared_ptr<HPTNode>> m_resetNodes;
        std::vector<std::shared_ptr<UISchedule>> m_schedules;
    };

    static void resetAllFromPack(std::string_view packName);

    static void resetChildren(std::string_view packName, CCNode* child);

    void setOwner(std::shared_ptr<HPTNode> node);

    void enableTouch();

    void resetByPack(std::string_view packName) ;

    void resetByPackJson(std::string_view packName);

    void setSchedule(std::shared_ptr<HPTNode> node);

    void setOnClick(std::shared_ptr<HPTNode> node);

    void setOnRelease(std::shared_ptr<HPTNode> node);

    void setOnActivate(std::shared_ptr<HPTNode> node);
    
    void setOnHover(std::shared_ptr<HPTNode> node);

    void setOnExit(std::shared_ptr<HPTNode> node);

    void setOnClick(const matjson::Value& value);

    void setOnRelease(const matjson::Value& value);

    void setOnActivate(const matjson::Value& value);

    void setOnHover(const matjson::Value& value);

    void setOnExit(const matjson::Value& value);
};

