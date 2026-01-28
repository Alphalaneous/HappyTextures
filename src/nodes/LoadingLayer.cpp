#include <Geode/Geode.hpp>
#include <Geode/modify/LoadingLayer.hpp>
#include "../Macros.hpp"
#include "../Config.hpp"
#include "../Utils.hpp"

using namespace geode::prelude;

class $modify(MyLoadingLayer, LoadingLayer) {
    
    static void onModify(auto& self) {
        (void) self.setHookPriorityPost("LoadingLayer::init", Priority::Late);
    }

    bool init(bool p1) {
        UIModding::get()->firstMenuLayer = true;
        Utils::clearCaches();
        Utils::reloadFileNames();
        UIModding::get()->loadNodeFiles();
        Config::get()->loadPackJsons();
        return LoadingLayer::init(p1);
    }
};
