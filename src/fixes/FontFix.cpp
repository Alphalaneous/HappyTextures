#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/CCLabelBMFont.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/CCSpriteBatchNode.hpp>
#include <Geode/modify/CCTextureCache.hpp>

using namespace geode::prelude;

#ifndef GEODE_IS_MACOS

bool s_isInCreateTextLayers = false;

class $modify(GJBaseGameLayer) {
    
    void createTextLayers() {
        s_isInCreateTextLayers = true;
        GJBaseGameLayer::createTextLayers();
        s_isInCreateTextLayers = false;
    }
};

class $modify(CCSpriteBatchNode) {
    
    bool initWithTexture(CCTexture2D* texture, unsigned int capacity) {
        bool doFix = Mod::get()->getSettingValue<bool>("pusab-fix");

        if (doFix) {
            if (s_isInCreateTextLayers && texture == CCTextureCache::sharedTextureCache()->addImage("bigFont.png", false)) {
                return CCSpriteBatchNode::initWithTexture(CCTextureCache::sharedTextureCache()->addImage("bigFont.png"_spr, false), capacity);
            }
        }
        return CCSpriteBatchNode::initWithTexture(texture, capacity);
    }
};


class $modify(CCLabelBMFont) {
    
    static CCLabelBMFont* createBatched(const char* str, const char* fntFile, CCArray* a, int a1) {

        bool doFix = Mod::get()->getSettingValue<bool>("pusab-fix");

        if (doFix) {
            if (strcmp(fntFile, "bigFont.fnt") == 0) {
                fntFile = "bigFont.fnt"_spr;
            }
        }
        return CCLabelBMFont::createBatched(str, fntFile, a, a1);
    }
};

class $modify(CCTextureCache) {
    CCTexture2D* addImage(const char* fileimage, bool p1) {
        CCTexture2D* ret = nullptr;
        bool didChange = false;
        if (strcmp(fileimage, "bigFont.png") == 0) {
            bool doFix = Mod::get()->getSettingValue<bool>("pusab-fix");

            if (doFix && (PlayLayer::get() || LevelEditorLayer::get())) {
                didChange = true;
                ret = CCTextureCache::addImage("bigFont.png"_spr, p1);
            }
        }
        if (!didChange) {
            ret = CCTextureCache::addImage(fileimage, p1);
        }
        return ret;
    }
};

#endif