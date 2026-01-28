#include <Geode/Geode.hpp>
#include "../Macros.hpp"
#include <alphalaneous.alphas_geode_utils/include/ObjectModify.hpp>
#include "../UIModding.hpp"
#include "../Utils.hpp"

using namespace geode::prelude;

class $classModify(MyGJListLayer, GJListLayer) {

	void modify() {
		if (UIModding::get()->doModify) {
			if (getColor() == ccColor3B{191,114,62}) {
                Utils::setColorIfExists(this, "list-layer-bg");
			}
		}
	}
};