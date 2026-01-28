#include <Geode/Geode.hpp>
#include "../Macros.hpp"
#include "../UIModding.hpp"
#include "../Utils.hpp"
#include <alphalaneous.alphas_geode_utils/include/ObjectModify.hpp>

using namespace geode::prelude;

class $classModify(MyMenuGameLayer, MenuGameLayer) {

    void modify() {
        MyMenuGameLayer::updateColors(0);
        schedule(schedule_selector(MyMenuGameLayer::updateColors));
    }

    void updateGroundSprite(CCSprite* spr) {
        if (!spr) return;
        std::optional<ColorData> dataOpt = UIModding::get()->getColors("main-menu-ground");
        if (dataOpt.has_value()) {
            for (CCNode* node : CCArrayExt<CCNode*>(spr->getChildren())) {
                if (CCSprite* child = typeinfo_cast<CCSprite*>(node)) {
                    ColorData data = dataOpt.value();
                    child->setColor(data.color);
                    child->setOpacity(data.alpha);
                }
            }
            ColorData data = dataOpt.value();
            spr->setColor(data.color);
            spr->setOpacity(data.alpha);
        }
    }

    void updateColors(float p0) {
        if (UIModding::get()->doModify) {
            auto self = reinterpret_cast<MenuGameLayer*>(this);

            Utils::setColorIfExists(self->m_backgroundSprite, "main-menu-bg");
        
            updateGroundSprite(self->m_groundLayer->m_ground1Sprite);
            updateGroundSprite(self->m_groundLayer->m_ground2Sprite);
        }
    }
};