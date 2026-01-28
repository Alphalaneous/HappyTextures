#include <Geode/Geode.hpp>
#include "../Callbacks.hpp"
#include "../Macros.hpp"
#include "../UIModding.hpp"
#include <alphalaneous.alphas_geode_utils/include/ObjectModify.hpp>

using namespace geode::prelude;

class $classModify(MyMenuLayer, MenuLayer) {

    void modify() {
        UIModding::get()->doModify = Mod::get()->getSettingValue<bool>("ui-modifications");
        Callbacks::get()->generateAll();

        if (UIModding::get()->doModify) {
            if (Mod::get()->getSettingValue<bool>("hot-reload")) {
                UIModding::get()->startFileListeners();
            }
        }
    }
};