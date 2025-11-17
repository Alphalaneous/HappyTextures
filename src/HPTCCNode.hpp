#pragma once

#include <Geode/Geode.hpp>
#include <alphalaneous.alphas_geode_utils/include/NodeModding.h>
#include "HPTParser.hpp"

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

class HPTCCNode;

struct TouchObject : public CCNode, CCTouchDelegate {
    CCNode* m_self;
    bool m_clicked;
    bool m_hovering;
    CCNode* m_parentLayer = nullptr;

    std::vector<std::shared_ptr<HPTNode>> m_onClick;
    std::vector<std::shared_ptr<HPTNode>> m_onRelease;
    std::vector<std::shared_ptr<HPTNode>> m_onActivate;
    std::vector<std::shared_ptr<HPTNode>> m_onHover;
    std::vector<std::shared_ptr<HPTNode>> m_onExit;

    static TouchObject* create(CCNode* self) {
        auto ret = new TouchObject();
        if (ret->init(self)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    CCNode* getSceneChildContainingNode() {
        if (m_parentLayer) {
            if (CCScene::get() != m_parentLayer->getParent()) return nullptr;
            return m_parentLayer;
        }

        auto current = m_self;
        while (current && current->getParent() != CCScene::get()) {
            current = current->getParent();
        }
        m_parentLayer = current;
        if (m_parentLayer && CCScene::get() != m_parentLayer->getParent()) return nullptr;
        return current;
    }

    bool isLastAlert() {
        bool shouldCheck = false;
        bool lastAlert = false;

        if (auto child = getSceneChildContainingNode()) {
            if (!child) return false;
            for (auto c : CCArrayExt<CCNode*>(child->getChildren())) {
                if (!c) continue;
                if (!AlphaUtils::Cocos::hasNode(m_self, c)) {
                    shouldCheck = true;
                }
                if (shouldCheck) {
                    if (typeinfo_cast<FLAlertLayer*>(c) || typeinfo_cast<CCBlockLayer*>(c)) {
                        if (AlphaUtils::Cocos::hasNode(m_self, c)) continue;
                        lastAlert = true;
                    }
                }
            }
        }
        return lastAlert;
    }

    bool isHoverable(CCNode* node, CCPoint point) {
        if (!CCScene::get() || !node || isLastAlert()) return false;

        auto sceneChild = getSceneChildContainingNode();
        if (!sceneChild) return false;

        for (auto child : CCArrayExt<CCNode*>(CCScene::get()->getChildren())) {
            if (child->getZOrder() <= sceneChild->getZOrder()) continue;
            if (child->boundingBox().containsPoint(point) && nodeIsVisible(child)) {
                return false;
            }
        }
        return true;
    }

    void checkMouse(float) {
        if (!nodeIsVisible(m_self)) return;
        if (!m_self->getParent()) return;
        
        auto worldPos = m_self->convertToWorldSpaceAR(CCPointZero);
        bool isValid = nodeIsVisible(m_self) && m_self->boundingBox().containsPoint(getMousePos()) && isHoverable(m_self, worldPos);

        checkTouch(!isValid);
    }

    void checkTouch(bool shouldExit) {
        if (!shouldExit && !m_hovering) {
            m_hovering = true;
            parseForEach(m_onHover);
        }
        if (shouldExit && m_hovering) {
            m_hovering = false;
            parseForEach(m_onExit);
        }
    }

    bool init(CCNode* self) {
        m_self = self;
        schedule(schedule_selector(TouchObject::checkMouse));
        return true;
    }

    void parseForEach(std::vector<std::shared_ptr<HPTNode>> vec) {
        for (const auto& v : vec) {
            v->reparse();
        }
    }

    bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent) override {
        if (m_self->boundingBox().containsPoint(pTouch->getLocation()) && getSceneChildContainingNode()) {
            parseForEach(m_onClick);
            m_clicked = true;
            return true;
        }
        return false;
    }

    void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent) override {
        if (m_self->boundingBox().containsPoint(pTouch->getLocation()) && getSceneChildContainingNode()) {
            if (!m_clicked) {
                m_clicked = true;
                parseForEach(m_onClick);
            }
        }
        else {
            if (m_clicked) {
                m_clicked = false;
                parseForEach(m_onRelease);
            }
        }
    }

    void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent) override {
        m_clicked = false;
        parseForEach(m_onActivate);
        parseForEach(m_onRelease);
    }

    void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent) override {
        m_clicked = false;
        parseForEach(m_onRelease);
    }

    void onEnter() override {
        CCNode::onEnter();
        CCDirector::get()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    }

    void onExit() override {
        CCNode::onExit();
        CCDirector::get()->getTouchDispatcher()->removeDelegate(this);
    }

    ~TouchObject() {
        unscheduleAllSelectors();
    }
};

class $nodeModify(HPTCCNode, CCNode) {

    struct Fields {
        TouchObject* m_touchObject;
        std::vector<std::shared_ptr<HPTNode>> m_owned;
        std::vector<std::shared_ptr<HPTNode>> m_resetNodes;
        std::vector<std::shared_ptr<UISchedule>> m_schedules;
    };

    static void resetAllFromPack(const std::string& packName) {
        auto scene = CCScene::get();
        if (!scene) return;
        resetChildren(packName, scene);
    }

    static void resetChildren(const std::string& packName, CCNode* child) {
        reinterpret_cast<HPTCCNode*>(child)->resetByPack(packName);
        for (auto node : CCArrayExt<CCNode*>(child->getChildren())) {
            resetChildren(packName, node);
        }
    }

    void modify() {}

    void setOwner(std::shared_ptr<HPTNode> node) {
        m_fields->m_owned.push_back(node);
    }

    void enableTouch() {
        auto fields = m_fields.self();
        if (!fields->m_touchObject) {
            fields->m_touchObject = TouchObject::create(this);
            addChild(fields->m_touchObject);
        }
    }

    void resetByPack(const std::string& packName) {
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
    }

    void setSchedule(std::shared_ptr<HPTNode> node) {
        auto fields = m_fields.self();
        CCActionInstant* callFunc = CallFuncExt::create([node] {

        });
        CCDelayTime* delay = CCDelayTime::create(0 /*pass in from node*/);
        CCSequence* sequence = CCSequence::create(callFunc, delay, nullptr);
        CCRepeatForever* repeat = CCRepeatForever::create(sequence);

        auto schedule = std::make_shared<UISchedule>(node, repeat);

        fields->m_schedules.push_back(schedule);
    }

    void setOnClick(std::shared_ptr<HPTNode> node) {
        auto fields = m_fields.self();
        enableTouch();
        fields->m_touchObject->m_onClick.push_back(node);
    }

    void setOnRelease(std::shared_ptr<HPTNode> node) {
        auto fields = m_fields.self();
        enableTouch();
        fields->m_touchObject->m_onRelease.push_back(node);
    }

    void setOnActivate(std::shared_ptr<HPTNode> node) {
        auto fields = m_fields.self();
        enableTouch();
        fields->m_touchObject->m_onActivate.push_back(node);
    }

    void setOnHover(std::shared_ptr<HPTNode> node) {
        auto fields = m_fields.self();
        enableTouch();
        fields->m_touchObject->m_onHover.push_back(node);
    }

    void setOnExit(std::shared_ptr<HPTNode> node) {
        auto fields = m_fields.self();
        enableTouch();
        fields->m_touchObject->m_onExit.push_back(node);
    }
};

