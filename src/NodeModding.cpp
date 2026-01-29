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

class $modify(HTCCObject, CCObject) {

    CCObject* autorelease() {
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
        return CCObject::autorelease();
    }
};

class $modify(CCPoolManager) {
    void pop() {
        LateQueue::get()->executeQueue();
        CCPoolManager::pop();
    }
};