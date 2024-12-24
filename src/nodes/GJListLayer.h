#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/GJListLayer.hpp>

using namespace geode::prelude;

class $modify(MyGJListLayer, GJListLayer) {

    static void onModify(auto& self) {
        HOOK_LATEST("GJListLayer::create");
    }

	static GJListLayer* create(BoomListView* p0, char const* p1, cocos2d::ccColor4B p2, float p3, float p4, int p5) {
		auto ret = GJListLayer::create(p0, p1, p2, p3, p4, p5);
		if (UIModding::get()->doModify) {
			if (ret->getColor() == ccColor3B{191,114,62}) {
                Utils::setColorIfExists(ret, "list-layer-bg");
			}
		}
		return ret;
	}
};