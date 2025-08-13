#pragma once

#include <Geode/Geode.hpp>
#include <alphalaneous.alphas_geode_utils/include/NodeModding.h>
#include "../Macros.hpp"
#include "../LateQueue.hpp"

using namespace geode::prelude;

class $nodeModify(ShardsPage) {
    void modify() {
        LateQueue::get()->queue(this, [this] {
            removeFromParentAndCleanup(false);
            CCDirector::get()->m_pRunningScene->addChild(this);
        });
    }
};
