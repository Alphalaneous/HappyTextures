#pragma once

#ifndef MYGJLISTLAYER_H
#define MYGJLISTLAYER_H

#include <Geode/Geode.hpp>
#include <Geode/modify/GJListLayer.hpp>

using namespace geode::prelude;

class $modify(MyGJListLayer, GJListLayer) {

    static void onModify(auto& self) {
        (void) self.setHookPriority("GJListLayer::create", INT_MIN);
    }

	static GJListLayer* create(BoomListView* p0, char const* p1, cocos2d::ccColor4B p2, float p3, float p4, int p5) {
		auto ret = GJListLayer::create(p0, p1, p2, p3, p4, p5);
		if(UIModding::get()->doModify){
			if(ret->getColor() == ccColor3B{191,114,62}){
				std::optional<ColorData> dataOpt = UIModding::get()->getColors("list-layer-bg");
				if(dataOpt.has_value()){
					ColorData data = dataOpt.value();
					ret->setColor(data.color);
					ret->setOpacity(data.alpha);
				}
			}
		}
		return ret;
	}
};

#endif