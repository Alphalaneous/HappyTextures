#include <Geode/Geode.hpp>
#include "../Callbacks.hpp"
#include "../Macros.hpp"
#include "../UIModding.hpp"
#include <Geode/ui/NineSlice.hpp>
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

        if (Mod::get()->getSettingValue<bool>("disable-texture-workshop-info")) return;

        auto label = CCLabelBMFont::create("Happy Textures is NOT Texture Workshop\nThere is no place to download packs with Happy Textures.\nYou can disable this message in Happy Textures' settings.", "bigFont.fnt");
        label->setZOrder(300);
        label->setScale(0.3f);
        label->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);

        auto winSize = CCDirector::get()->getWinSize();

        label->setPosition({winSize.width / 2, 90});

        addChild(label);

        auto noticeBg = geode::NineSlice::create("geode.loader/white-square-uhd.png");
        noticeBg->setContentSize(label->getScaledContentSize() + CCSize{10, 10});
        noticeBg->setColor({127, 0, 0});
        noticeBg->setPosition(label->getPosition());
        noticeBg->setZOrder(299);

        addChild(noticeBg);

        auto textureWorkshop = Loader::get()->getInstalledMod("uproxide.textures");
        if (!textureWorkshop) return; 

        if (!textureWorkshop->isLoaded() && textureWorkshop->targetsOutdatedVersion()) {
            auto rightMenu = getChildByID("right-side-menu");
            if (rightMenu) {
                auto btn = CCMenuItemExt::createSpriteExtraWithFilename("TWS_MainButton.png"_spr, 1.f, [] (auto sender) {
                    createQuickPopup("Whoops!", "<cb>Texture Workshop</c> has not been updated to support your version of Geometry Dash yet!\nThis alert was created by <cg>Happy Textures</c>. <cg>Happy Textures</c> is <cr>NOT</c> <cb>Texture Workshop</c>, please <cr>stop</c> reporting that you can't download packs with <cg>Happy Textures</c>.", "OK", nullptr, nullptr);
                });
                rightMenu->addChild(btn);
                rightMenu->updateLayout();
            }
        }
    }
};