#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCObject.hpp>
#include "UIModding.hpp"
#include "nodes/CCNode.hpp"
#include "Utils.hpp"
#include "Macros.hpp"

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
