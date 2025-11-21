#include "TouchObject.hpp"
#include <alphalaneous.alphas_geode_utils/include/Utils.h>
#include "UIModding.hpp"
#include "HPTCCNode.hpp"

TouchObject* TouchObject::create(HPTCCNode* self) {
    auto ret = new TouchObject();
    if (ret->init(self)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

CCNode* TouchObject::getSceneChildContainingNode() {
    if (m_parentLayer) {
        if (CCScene::get() != m_parentLayer->getParent()) return nullptr;
        return m_parentLayer;
    }

    CCNode* current = m_self;
    while (current && current->getParent() != CCScene::get()) {
        current = current->getParent();
    }
    m_parentLayer = current;
    if (m_parentLayer && CCScene::get() != m_parentLayer->getParent()) return nullptr;
    return current;
}

bool TouchObject::isLastAlert() {
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

bool TouchObject::isHoverable(CCNode* node) {
    if (!CCScene::get() || !node || isLastAlert()) return false;
    auto worldPos = node->convertToWorldSpaceAR(CCPointZero);

    auto sceneChild = getSceneChildContainingNode();
    if (!sceneChild) return false;

    for (auto child : CCArrayExt<CCNode*>(CCScene::get()->getChildren())) {
        if (child->getZOrder() <= sceneChild->getZOrder()) continue;
        if (child->boundingBox().containsPoint(worldPos) && nodeIsVisible(child)) {
            return false;
        }
    }
    return true;
}

void TouchObject::checkMouse(float) {
    if (!nodeIsVisible(m_self)) return;
    if (!m_self->getParent()) return;
    
    auto nodeMouse = m_self->getParent()->convertToNodeSpace(getMousePos());

    bool isValid = m_self->boundingBox().containsPoint(nodeMouse) && isHoverable(m_self);

    checkTouch(!isValid);
}

void TouchObject::checkTouch(bool shouldExit) {
    if (!shouldExit && !m_hovering) {
        m_hovering = true;
        parseForEach(m_onHover);
        parseForEach(m_self, m_onHoverJson);
    }
    if (shouldExit && m_hovering) {
        m_hovering = false;
        parseForEach(m_onExit);
        parseForEach(m_self, m_onExitJson);
    }
}

bool TouchObject::init(HPTCCNode* self) {
    m_self = self;
    schedule(schedule_selector(TouchObject::checkMouse));
    return true;
}

void TouchObject::parseForEach(std::vector<std::shared_ptr<HPTNode>> vec) {
    for (const auto& v : vec) {
        v->reparse();
    }
}

void TouchObject::parseForEach(HPTCCNode* node, const std::vector<matjson::Value>& values) {
    for (const matjson::Value& value : values) {
        UIModding::get()->handleModifications(node, value);
    }
}

void TouchObject::activate(CCNode* node) {
    auto hpt = static_cast<HPTCCNode*>(node);
    auto fields = hpt->m_fields.self();

    if (auto touch = fields->m_touchObject) {
        if (auto item = typeinfo_cast<CCMenuItem*>(node)) {
            if (!touch->m_cancelOriginalActivate) item->activate();
        }

        parseForEach(touch->m_onActivate);
        parseForEach(hpt, touch->m_onActivateJson);
    }
    else {
        if (auto item = typeinfo_cast<CCMenuItem*>(node)) {
            item->activate();
        }
    }
}

void TouchObject::selected(CCNode* node) {
    auto hpt = static_cast<HPTCCNode*>(node);
    auto fields = hpt->m_fields.self();

    if (auto touch = fields->m_touchObject) {
        if (auto item = typeinfo_cast<CCMenuItem*>(node)) {
            if (!touch->m_cancelOriginalClick) item->selected();
        }

        parseForEach(touch->m_onClick);
        parseForEach(hpt, touch->m_onClickJson);
    }
    else {
        if (auto item = typeinfo_cast<CCMenuItem*>(node)) {
            item->selected();
        }
    }
}

void TouchObject::unselected(CCNode* node) {
    auto hpt = static_cast<HPTCCNode*>(node);
    auto fields = hpt->m_fields.self();

    if (auto touch = fields->m_touchObject) {
        if (auto item = typeinfo_cast<CCMenuItem*>(node)) {
            if (!touch->m_cancelOriginalRelease) item->unselected();
        }
        parseForEach(touch->m_onRelease);
        parseForEach(hpt, touch->m_onReleaseJson);
    }
    else {
        if (auto item = typeinfo_cast<CCMenuItem*>(node)) {
            item->unselected();
        }
    }
}

void TouchObject::hovered(CCNode* node) {
    auto hpt = static_cast<HPTCCNode*>(node);

    auto fields = hpt->m_fields.self();
    if (auto touch = fields->m_touchObject) {
        parseForEach(touch->m_onHover);
        parseForEach(hpt, touch->m_onHoverJson);
    }
}

void TouchObject::exited(CCNode* node) {
    auto hpt = static_cast<HPTCCNode*>(node);

    auto fields = hpt->m_fields.self();
    if (auto touch = fields->m_touchObject) {
        parseForEach(touch->m_onExit);
        parseForEach(hpt, touch->m_onExitJson);
    }
}

CCMenuItem* TouchObject::getOnAnotherButton(CCPoint pos) {
    if (auto menu = typeinfo_cast<CCMenu*>(m_self->getParent())) {
        for (auto child : menu->getChildrenExt()) {
            if (auto item = typeinfo_cast<CCMenuItem*>(child)) {
                if (item->boundingBox().containsPoint(pos) && getSceneChildContainingNode() && nodeIsVisible(item)) {
                    return item;
                }
            }
        }
    }
    return nullptr;
}

void TouchObject::handleBegan(CCNode* node) {
    if (!node) node = m_self;

    selected(node);

    #ifndef GEODE_IS_DESKTOP
    hovered(node);
    #endif
    m_clicked[node] = true;
}

void TouchObject::handleMoved(CCPoint pos, CCNode* node) {
    if (!node) node = m_self;

    if (node->boundingBox().containsPoint(pos) && getSceneChildContainingNode()) {
        if (!m_clicked[node]) {
            m_clicked[node] = true;
            selected(node);
            #ifndef GEODE_IS_DESKTOP
            hovered(node);
            #endif
        }
    }
    else {
        if (m_clicked[node]) {
            m_clicked[node] = false;
            unselected(node);
            #ifndef GEODE_IS_DESKTOP
            exited(node);
            #endif
        }
    }
}

void TouchObject::handleEnded(CCPoint pos, CCNode* node) {
    if (!node) node = m_self;

    m_clicked[node] = false;

    if (node->boundingBox().containsPoint(pos) && getSceneChildContainingNode()) {
        activate(node);
    }

    unselected(node);
    #ifndef GEODE_IS_DESKTOP
    exited(node);
    #endif
}

void TouchObject::handleCancelled(CCNode* node) {
    if (!node) node = m_self;

    m_clicked[node] = false;

    unselected(node);
    #ifndef GEODE_IS_DESKTOP
    exited(node);
    #endif
}

bool TouchObject::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent) {
    auto pos = m_self->getParent()->convertToNodeSpace(pTouch->getLocation());

    if (auto node = typeinfo_cast<CCTouchDelegate*>(m_self)) {
        m_propagateOriginalTouch = node->ccTouchBegan(pTouch, pEvent);
    }

    if (m_self->boundingBox().containsPoint(pos) && getSceneChildContainingNode() && nodeIsVisible(m_self)) {
        handleBegan();
        return true;
    }
    else {
        if (auto item = getOnAnotherButton(pos)) {
            handleBegan(item);
            return true;
        }
    }
    return false;
}

void TouchObject::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent) {
    auto pos = m_self->getParent()->convertToNodeSpace(pTouch->getLocation());

    if (auto node = typeinfo_cast<CCTouchDelegate*>(m_self)) {
        if (m_propagateOriginalTouch) node->ccTouchMoved(pTouch, pEvent);
    }

    if (auto menu = typeinfo_cast<CCMenu*>(m_self->getParent())) {
        for (auto child : menu->getChildrenExt()) {
            if (auto item = typeinfo_cast<CCMenuItem*>(child)) {
                handleMoved(pos, item);
            }
        }
    }
    else {
        handleMoved(pos);
    }
}

void TouchObject::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent) {
    if (auto node = typeinfo_cast<CCTouchDelegate*>(m_self)) {
        if (m_propagateOriginalTouch) node->ccTouchEnded(pTouch, pEvent);
    }

    auto pos = m_self->getParent()->convertToNodeSpace(pTouch->getLocation());

    if (m_self->boundingBox().containsPoint(pos) && getSceneChildContainingNode()) {
        handleEnded(pos);
    }
    else {
        if (auto item = getOnAnotherButton(pos)) {
            handleEnded(pos, item);
        }
    }
    m_clicked.clear();
}

void TouchObject::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent) {

    if (auto node = typeinfo_cast<CCTouchDelegate*>(m_self)) {
        if (m_propagateOriginalTouch) node->ccTouchCancelled(pTouch, pEvent);
    }

    auto pos = m_self->getParent()->convertToNodeSpace(pTouch->getLocation());

    if (m_self->boundingBox().containsPoint(pos) && getSceneChildContainingNode()) {
        handleCancelled();
    }
    else {
        if (auto item = getOnAnotherButton(pos)) {
            handleCancelled(item);
        }
    }
    m_clicked.clear();
}

void TouchObject::onEnter() {
    CCNode::onEnter();

    auto prio = 0;
    if (auto delegate = typeinfo_cast<CCTouchDelegate*>(m_self)) {
        if (auto handler = CCTouchDispatcher::get()->findHandler(delegate)) {
            prio = handler->getPriority();
        }
    }

    if (auto delegate = typeinfo_cast<CCMenu*>(m_self->getParent())) {
        if (auto handler = CCTouchDispatcher::get()->findHandler(delegate)) {
            prio = handler->getPriority();
        }
    }

    CCDirector::get()->getTouchDispatcher()->addTargetedDelegate(this, prio - 1, true);
}

void TouchObject::onExit() {
    CCNode::onExit();
    CCDirector::get()->getTouchDispatcher()->removeDelegate(this);
}

TouchObject::~TouchObject() {
    unscheduleAllSelectors();
}