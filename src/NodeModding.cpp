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

class NodeModding : public CCObject {
public:
    static NodeModding* create() {
        auto ret = new NodeModding();
        ret->autorelease();
        return ret;
    }

    static NodeModding* get() {
        static NodeModding* instance = nullptr;
        if (!instance) {
            instance = NodeModding::create();
        }
        return instance;
    }

    void handleCurrentNode(CCNode* node) {
        std::string className = Utils::getNodeName(node);
        UIModding::get()->doUICheckForType(className, node);
    }

    void handleArray(CCArray* array) {
        auto scene = CCDirector::sharedDirector()->getRunningScene();

        if (scene && UIModding::get()->doModify && UIModding::get()->finishedLoad && !Callbacks::get()->m_ignoreUICheck) {
            
            for (auto object : CCArrayExt<CCObject*>(array)) {
                if (auto node = typeinfo_cast<CCNode*>(object)) {
                    handleCurrentNode(node);
                }
            }
        }
    }
};

#include <Geode/modify/CCPoolManager.hpp>

class $modify(CCPoolManager) {
    void pop() {
        auto poolManager = reinterpret_cast<CCPoolManagerHack*>(this);

        if (!poolManager || !poolManager->m_pCurReleasePool) return;
        auto pool = reinterpret_cast<CCAutoreleasePoolHack*>(poolManager->m_pCurReleasePool);

        if (!pool->m_pManagedObjectArray) return;

        NodeModding::get()->handleArray(pool->m_pManagedObjectArray);

        return CCPoolManager::pop();
    }
};