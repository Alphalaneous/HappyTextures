#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCObject.hpp>
#include "UIModding.h"
#include "Utils.h"
#include "Macros.h"
#include "Callbacks.h"

using namespace geode::prelude;

class CCAutoreleasePoolHack : public CCObject {
public:
    CCArray* m_pManagedObjectArray;
};

class CCPoolManagerHack {
public:
    CCArray* m_pReleasePoolStack;
    CCAutoreleasePool* m_pCurReleasePool;
};

class NodeModding {

protected:
    std::unordered_map<std::string, std::pair<std::function<void(CCNode*)>, bool>> m_nodesToModify;

public:
    static NodeModding* get() {
        static NodeModding* instance = nullptr;
        if (!instance) {
            instance = new NodeModding();
        }
        return instance;
    }

    std::unordered_map<std::string, std::pair<std::function<void(CCNode*)>, bool>> getNodesToModify() {
        return m_nodesToModify;
    }

    void addNodeToModify(std::string className, std::function<void(CCNode*)> func) {
        m_nodesToModify[className] = {func, true};
    }

    void handleCurrentNode(CCNode* node) {
        std::string className = Utils::getNodeName(node);
        if (m_nodesToModify.contains(className)) {
            std::pair<std::function<void(CCNode*)>, bool> nodeToModify = m_nodesToModify[className];
            if (!node->getUserObject("node_modified"_spr)) {
                if (!nodeToModify.second) {
                    nodeToModify.first(node);
                }
                node->setUserObject("node_modified"_spr, CCBool::create(true));
            }
        }
        if (UIModding::get()->finishedLoad) {
            UIModding::get()->doUICheckForType(className, node);
        }
    }
};

#include <Geode/modify/CCObject.hpp>

class $modify(CCObject) {

    // fuck it we ball (we end up needing this if we don't want a frame delay, aw shucks)
    CCObject* autorelease() {
        if (CCNode* node = typeinfo_cast<CCNode*>(this)) {
            std::string className = Utils::getNodeName(node);
            if (NodeModding::get()->getNodesToModify().contains(className)) {
                if (!node->getUserObject("node_modified"_spr)) {
                    if (NodeModding::get()->getNodesToModify()[className].second) {
                        NodeModding::get()->getNodesToModify()[className].first(node);
                    }
                    node->setUserObject("node_modified"_spr, CCBool::create(true));
                }
            }
            NodeModding::get()->handleCurrentNode(node);
        }
        return CCObject::autorelease();
    }
};
