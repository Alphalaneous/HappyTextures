#pragma once

#include <Geode/Geode.hpp>
#include "HPTParser.hpp"

using namespace geode::prelude;

class HPTCCNode;

struct TouchObject : public CCNode, CCTouchDelegate {
    HPTCCNode* m_self;
    std::unordered_map<Ref<CCNode>, bool> m_clicked;
    bool m_hovering;
    bool m_propagateOriginalTouch;
    bool m_cancelOriginalClick;
    bool m_cancelOriginalRelease;
    bool m_cancelOriginalActivate;

    CCNode* m_parentLayer = nullptr;

    std::vector<std::shared_ptr<HPTNode>> m_onClick;
    std::vector<std::shared_ptr<HPTNode>> m_onRelease;
    std::vector<std::shared_ptr<HPTNode>> m_onActivate;
    std::vector<std::shared_ptr<HPTNode>> m_onHover;
    std::vector<std::shared_ptr<HPTNode>> m_onExit;

    std::vector<matjson::Value> m_onClickJson;
    std::vector<matjson::Value> m_onReleaseJson;
    std::vector<matjson::Value> m_onActivateJson;
    std::vector<matjson::Value> m_onHoverJson;
    std::vector<matjson::Value> m_onExitJson;

    static TouchObject* create(HPTCCNode* self);

    CCNode* getSceneChildContainingNode();
    bool isLastAlert();

    bool isHoverable(CCNode* node);
    void checkMouse(float);
    void checkTouch(bool shouldExit);
    bool init(HPTCCNode* self);

    void parseForEach(std::vector<std::shared_ptr<HPTNode>> vec);
    void parseForEach(HPTCCNode* node, const std::vector<matjson::Value>& values);

    void activate(CCNode* node);
    void selected(CCNode* node);
    void unselected(CCNode* node);
    void hovered(CCNode* node);
    void exited(CCNode* node);

    CCMenuItem* getOnAnotherButton(CCPoint pos);

    bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent) override;
    void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent) override;
    void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent) override;
    void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent) override;

    void handleBegan(CCNode* node = nullptr);
    void handleMoved(CCPoint pos, CCNode* node = nullptr);
    void handleEnded(CCPoint pos, CCNode* node = nullptr);
    void handleCancelled(CCNode* node = nullptr);

    void onEnter() override;
    void onExit() override;

    ~TouchObject();
};