#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCObject.hpp>
#include <Geode/modify/CCPoolManager.hpp>
#include <Geode/modify/CCDictionary.hpp>
#include "UIModding.hpp"
#include "nodes/CCNode.hpp"
#include <alphalaneous.alphas_geode_utils/include/NodeModding.h>
#include <alphalaneous.alphas_geode_utils/include/Fields.h>
#include "Utils.hpp"
#include "Macros.hpp"
#include "LateQueue.hpp"

using namespace geode::prelude;

class $modify(CCDictionary) {

    // Cocos will call autorelease on a nullptr here for some reason, 
    // it doesn't crash due to no member access but will crash with this 
    // mod since I retain and release which access m_uReference 

    static CCDictionary* createWithContentsOfFile(const char *pFileName) {
        if (auto ret = CCDictionary::createWithContentsOfFileThreadSafe(pFileName)) {
            ret->autorelease();
            return ret;
        }

        return nullptr;
    }
};

static std::unordered_map<CCObject*, uintptr_t> s_nodeVTables;

static bool checkNodeValidity(CCNode* node) {
    auto ret = *(uintptr_t*)node == s_nodeVTables[node];
    s_nodeVTables.erase(node);
    return ret;
}

static void setNodeVTable(CCNode* node) {
    s_nodeVTables[node] = *(uintptr_t*)node;
}

class $modify(HTCCObject, CCObject) {

    CCObject* autorelease() {
        auto modding = UIModding::get();
        if (!modding->doModify) return CCObject::autorelease();
        
        if (MyCCNode* node = static_cast<MyCCNode*>(typeinfo_cast<CCNode*>(this))) {
            if (!node->isModified()) {
                setNodeVTable(node);
                node->retain();
                LateQueue::get()->queue(node, [modding, node] {
                    if (!checkNodeValidity(node)) return;
                    std::string className = AlphaUtils::Cocos::getClassName(node, true);
                    modding->doUICheckForType(className, node);
                    node->setModified();
                    node->release();
                });
            }
        }
        return CCObject::autorelease();
    }
};

class $modify(CCPoolManager) {
    void pop() {
        LateQueue::get()->executeQueue();
        CCPoolManager::pop();
    }
};