#include <Geode/Geode.hpp>
#include <Geode/modify/CCObject.hpp>
#include "UIModding.h"
#include "Utils.h"
#include "Macros.h"

using namespace geode::prelude;

class $modify(MyCCObject, CCObject) {

    static void onModify(auto& self) {
        HOOK_LATEST("cocos2d::CCObject::autorelease");
    }

    CCObject* autorelease() {
        if (!UIModding::get()->finishedLoad || !UIModding::get()->doModify) 
            return CCObject::autorelease();
        
        if (CCNode* node = typeinfo_cast<CCNode*>(this)) {
            node->retain();
            std::string className = Utils::getNodeName(this);
            queueInMainThread([=] {
                UIModding::get()->doUICheckForType(className, node);
                node->release();
            });
        }
        
        return CCObject::autorelease();
    }
};