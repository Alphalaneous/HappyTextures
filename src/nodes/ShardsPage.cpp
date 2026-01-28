#include <Geode/Geode.hpp>
#include <alphalaneous.alphas_geode_utils/include/ObjectModify.hpp>
#include "../Macros.hpp"
#include "../LateQueue.hpp"

using namespace geode::prelude;

class $classModify(ShardsPage) {
    void modify() {
        LateQueue::get()->queue(this, [this] {
            removeFromParentAndCleanup(false);
            CCDirector::get()->m_pRunningScene->addChild(this);
        });
    }
};
