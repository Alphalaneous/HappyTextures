#pragma once

#include <Geode/Geode.hpp>
#include "Pack.h"
#include <geode.texture-loader/include/TextureLoader.hpp>

using namespace geode::prelude;

class PackSettingsPopup : public geode::Popup<> {

protected:
    bool setup() override;
    bool init(PackInfo pack);
    geode::texture_loader::Pack m_pack;
public:
    static PackSettingsPopup* create(PackInfo pack);
    void applySettings(CCObject* obj);
    void resetAll(CCObject* obj);
    void openFolder(CCObject* obj);
    bool isZipped();
};