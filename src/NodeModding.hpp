#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCObject.hpp>
#include <Geode/modify/CCPoolManager.hpp>
#include <Geode/modify/CCDictionary.hpp>
#include "nodes/CCNode.hpp"
#include <alphalaneous.alphas_geode_utils/include/NodeModding.h>
#include <alphalaneous.alphas_geode_utils/include/Fields.h>
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

static bool checkNodeValidity(CCObject* node) {
    auto ret = *(uintptr_t*)node == s_nodeVTables[node];
    s_nodeVTables.erase(node);
    return ret;
}

static void setNodeVTable(CCNode* node) {
    s_nodeVTables[node] = *(uintptr_t*)node;
}

static std::unordered_map<const std::type_info*, bool> s_isNodeCache;

static bool isNode(CCObject* obj) {
    const std::type_info* ti = &typeid(*obj);

    auto it = s_isNodeCache.find(ti);
    if (it != s_isNodeCache.end()) {
        return it->second;
    }

    bool isNode = typeinfo_cast<CCNode*>(obj) != nullptr;
    s_isNodeCache.emplace(ti, isNode);

    return isNode;
}

class $modify(HTCCObject, CCObject) {

    static std::string_view extract(std::string_view s) {
        if (auto pos = s.rfind("::"); pos != std::string_view::npos)
            return s.substr(pos + 2);
        return s;
    }

    CCObject* autorelease() {
        auto modding = UIModding::get();
        if (!modding->doModify) return CCObject::autorelease();

        if (isNode(this)) {
            MyCCNode* node = reinterpret_cast<MyCCNode*>(this);
            
            auto fields = node->m_fields.self();
            if (!fields->m_modified) {
                setNodeVTable(node);
                LateQueue::get()->queue(node, [modding, node = Ref(node), fields] {
                    if (!checkNodeValidity(node)) return;
                    modding->doUICheckForType(extract(cocos::getObjectName(node)), node);
                    fields->m_modified = true;
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