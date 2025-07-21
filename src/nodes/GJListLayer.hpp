#pragma once

#include <Geode/Geode.hpp>
#include "../Macros.hpp"
#include <alphalaneous.alphas_geode_utils/include/NodeModding.h>
#include "../UIModding.hpp"
#include "../Utils.hpp"

using namespace geode::prelude;

class $nodeModify(MyGJListLayer, GJListLayer) {

	void modify() {
		auto self = reinterpret_cast<GJListLayer*>(this);
		if (UIModding::get()->doModify) {
			if (self->getColor() == ccColor3B{191,114,62}) {
                Utils::setColorIfExists(self, "list-layer-bg");
			}
		}
	}
};