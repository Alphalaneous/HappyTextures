#include <Geode/Geode.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/modify/CCObject.hpp>
#include <Geode/modify/CCPoolManager.hpp>
#include <Geode/modify/CCDictionary.hpp>
#include "nodes/CCNode.hpp"
#include "Macros.hpp"
#include "LateQueue.hpp"
#include "Utils.hpp"

using namespace geode::prelude;

class $modify(HTCCObject, CCObject) {

    CCObject* autorelease() {
        auto ret = CCObject::autorelease();
        if (!ret) return nullptr;

        auto modding = UIModding::get();
        if (!modding->doModify) return CCObject::autorelease();

        if (modding->isNode(this)) {
            MyCCNode* node = reinterpret_cast<MyCCNode*>(this);
            
            auto fields = node->m_fields.self();
            fields->m_self = node;

            if (!fields->m_modified) {
                modding->setNodeVTable(node);
                LateQueue::get()->queue(node, [modding, node = Ref(node), fields] {
                    if (!modding->checkNodeValidity(node)) return;
                    modding->doUICheckForType(Utils::extract(std::string(cocos::getObjectName(node))), node);
                    fields->m_modified = true;
                });
            }
        }
        return ret;
    }
};

class $modify(CCPoolManager) {
    void pop() {
        LateQueue::get()->executeQueue();
        CCPoolManager::pop();
    }
};