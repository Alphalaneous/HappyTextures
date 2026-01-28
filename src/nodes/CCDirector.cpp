#include <Geode/Geode.hpp>
#include <Geode/modify/CCDirector.hpp>
#include "../UIModding.hpp"

using namespace geode::prelude;

class $modify(MyCCDirector, CCDirector) {

    void willSwitchToScene(CCScene* scene) {
        CCDirector::willSwitchToScene(scene);
        UIModding::get()->reloadChildren(scene, true);
    }
};
