#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCObject.hpp>
#include "UIModding.h"
#include "nodes/CCNode.h"
#include "Utils.h"
#include "Macros.h"
#include "Callbacks.h"

using namespace geode::prelude;

class $modify(CCObject) {

    CCObject* autorelease() {
        auto modding = UIModding::get();
        if (modding->doModify) {
            if (MyCCNode* node = static_cast<MyCCNode*>(typeinfo_cast<CCNode*>(this))) {
                const std::string& className = Utils::getNodeName(node);
                if (modding->finishedLoad && !node->isModified()) {
                    modding->doUICheckForType(className, node);
                    node->setModified();
                }
            }
        }
        return CCObject::autorelease();
    }
};
