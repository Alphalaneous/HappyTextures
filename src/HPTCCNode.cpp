#include "HPTCCNode.hpp"

void HPTCCNode::resetAllFromPack(ZStringView packName) {
    auto scene = CCScene::get();
    if (!scene) return;
    resetChildren(packName, scene);
}

void HPTCCNode::resetChildren(ZStringView packName, CCNode* child) {
    reinterpret_cast<HPTCCNode*>(child)->resetByPack(packName);
    for (auto node : CCArrayExt<CCNode*>(child->getChildren())) {
        resetChildren(packName, node);
    }
}

void HPTCCNode::setOwner(std::shared_ptr<HPTNode> node) {
    m_fields->m_owned.push_back(node);
}

void HPTCCNode::enableTouch() {
    auto fields = m_fields.self();
    if (!fields->m_touchObject) {
        if (auto nodeRGBA = typeinfo_cast<CCNodeRGBA*>(this)) {
            nodeRGBA->setUserObject("orig-color"_spr, ObjWrapper<ccColor3B>::create(nodeRGBA->getColor()));
            nodeRGBA->setUserObject("orig-opacity"_spr, CCInteger::create(nodeRGBA->getOpacity()));
        }
        fields->m_touchObject = TouchObject::create(this);
        addChild(fields->m_touchObject);
    }
}

void HPTCCNode::resetByPack(ZStringView packName) {
    auto fields = m_fields.self();

    auto erase = [&packName] (std::vector<std::shared_ptr<HPTNode>>& vec) {
        vec.erase(
            std::remove_if(vec.begin(), vec.end(), [&packName](std::shared_ptr<HPTNode> node){ return node->packName == packName; }),
            vec.end()
        );
    };

    for (auto action : fields->m_schedules) {
        if (action->m_node->packName == packName) {
            stopAction(action->get());
        }
    }

    fields->m_schedules.erase(
        std::remove_if(fields->m_schedules.begin(), fields->m_schedules.end(), [&packName] (std::shared_ptr<UISchedule> action) { return action->m_node->packName == packName; }),
        fields->m_schedules.end()
    );

    fields->m_owned.erase(
        std::remove_if(fields->m_owned.begin(), fields->m_owned.end(), [&packName] (std::shared_ptr<HPTNode> node) { return node->packName == packName; }),
        fields->m_owned.end()
    );

    if (fields->m_touchObject) {
        erase(fields->m_touchObject->m_onClick);
        erase(fields->m_touchObject->m_onRelease);
        erase(fields->m_touchObject->m_onActivate);
        erase(fields->m_touchObject->m_onHover);
        erase(fields->m_touchObject->m_onExit);
    }

    //resetByPackJson(packName);
}

void HPTCCNode::resetByPackJson(ZStringView packName) {
    auto fields = m_fields.self();

    auto erase = [&packName] (std::vector<matjson::Value>& vec) {
        vec.erase(
            std::remove_if(vec.begin(), vec.end(), [&packName](matjson::Value value){ return value["_pack-name"] == packName; }),
            vec.end()
        );
    };

    if (fields->m_touchObject) {
        erase(fields->m_touchObject->m_onClickJson);
        erase(fields->m_touchObject->m_onReleaseJson);
        erase(fields->m_touchObject->m_onActivateJson);
        erase(fields->m_touchObject->m_onHoverJson);
        erase(fields->m_touchObject->m_onExitJson);
    }
}

void HPTCCNode::setSchedule(std::shared_ptr<HPTNode> node) {
    auto fields = m_fields.self();
    CCActionInstant* callFunc = CallFuncExt::create([node] {

    });
    CCDelayTime* delay = CCDelayTime::create(0 /*pass in from node*/);
    CCSequence* sequence = CCSequence::create(callFunc, delay, nullptr);
    CCRepeatForever* repeat = CCRepeatForever::create(sequence);

    auto schedule = std::make_shared<UISchedule>(node, repeat);

    fields->m_schedules.push_back(schedule);
}

void HPTCCNode::setOnClick(std::shared_ptr<HPTNode> node) {
    auto fields = m_fields.self();
    enableTouch();
    fields->m_touchObject->m_onClick.push_back(node);
}

void HPTCCNode::setOnRelease(std::shared_ptr<HPTNode> node) {
    auto fields = m_fields.self();
    enableTouch();
    fields->m_touchObject->m_onRelease.push_back(node);
}

void HPTCCNode::setOnActivate(std::shared_ptr<HPTNode> node) {
    auto fields = m_fields.self();
    enableTouch();
    fields->m_touchObject->m_onActivate.push_back(node);
}

void HPTCCNode::setOnHover(std::shared_ptr<HPTNode> node) {
    auto fields = m_fields.self();
    enableTouch();
    fields->m_touchObject->m_onHover.push_back(node);
}

void HPTCCNode::setOnExit(std::shared_ptr<HPTNode> node) {
    auto fields = m_fields.self();
    enableTouch();
    fields->m_touchObject->m_onExit.push_back(node);
}

void HPTCCNode::setOnClick(const matjson::Value& value) {
    auto fields = m_fields.self();
    enableTouch();
    fields->m_touchObject->m_onClickJson.push_back(value);
    auto& override = fields->m_touchObject->m_cancelOriginalClick;

    if (!override) {
        if (value.contains("override")) {
            matjson::Value overrideVal = value["override"];
            if (overrideVal.isBool()) {
                override = overrideVal.asBool().unwrapOr(false);
            }
        }
    }
}

void HPTCCNode::setOnRelease(const matjson::Value& value) {
    auto fields = m_fields.self();
    enableTouch();
    fields->m_touchObject->m_onReleaseJson.push_back(value);

    auto& override = fields->m_touchObject->m_cancelOriginalRelease;

    if (!override) {
        if (value.contains("override")) {
            matjson::Value overrideVal = value["override"];
            if (overrideVal.isBool()) {
                override = overrideVal.asBool().unwrapOr(false);
            }
        }
    }
}

void HPTCCNode::setOnActivate(const matjson::Value& value) {
    auto fields = m_fields.self();
    enableTouch();
    fields->m_touchObject->m_onActivateJson.push_back(value);

    auto& override = fields->m_touchObject->m_cancelOriginalActivate;

    if (!override) {
        if (value.contains("override")) {
            matjson::Value overrideVal = value["override"];
            if (overrideVal.isBool()) {
                override = overrideVal.asBool().unwrapOr(false);
            }
        }
    }
}

void HPTCCNode::setOnHover(const matjson::Value& value) {
    auto fields = m_fields.self();
    enableTouch();
    fields->m_touchObject->m_onHoverJson.push_back(value);
}

void HPTCCNode::setOnExit(const matjson::Value& value) {
    auto fields = m_fields.self();
    enableTouch();
    fields->m_touchObject->m_onExitJson.push_back(value);
}