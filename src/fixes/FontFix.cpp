#include <Geode/Geode.hpp>
#include <Geode/modify/CCLabelBMFont.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/CCSpriteBatchNode.hpp>
#include <Geode/modify/CCTextureCache.hpp>

using namespace geode::prelude;

bool s_isInCreateTextLayers = false;

class $modify(GJBaseGameLayer) {
    
    void createTextLayers() {
        s_isInCreateTextLayers = true;
        GJBaseGameLayer::createTextLayers();
        s_isInCreateTextLayers = false;
    }
};

static bool pusabFix() {
    static bool setting = Mod::get()->getSettingValue<bool>("pusab-fix");
    static auto listener = listenForSettingChanges("pusab-fix", [](bool value) {
        setting = value;
    });

    return setting;
}

class $modify(CCSpriteBatchNode) {
    
    bool initWithTexture(CCTexture2D* texture, unsigned int capacity) {
        if (!pusabFix()) return CCSpriteBatchNode::initWithTexture(texture, capacity);

        if (s_isInCreateTextLayers && texture == CCTextureCache::sharedTextureCache()->addImage("bigFont.png", false)) {
            return CCSpriteBatchNode::initWithTexture(CCTextureCache::sharedTextureCache()->addImage("bigFont.png"_spr, false), capacity);
        }
        
        return CCSpriteBatchNode::initWithTexture(texture, capacity);
    }
};

class $modify(CCLabelBMFont) {
    
    static CCLabelBMFont* createBatched(const char* str, const char* fntFile, CCArray* a, int a1) {
        if (!pusabFix()) return CCLabelBMFont::createBatched(str, fntFile, a, a1);
        if (std::string_view(fntFile) == "bigFont.fnt") {
            return CCLabelBMFont::createBatched(str, "bigFont.fnt"_spr, a, a1);
        }
        
        return CCLabelBMFont::createBatched(str, fntFile, a, a1);
    }
};

class $modify(CCTextureCache) {
    CCTexture2D* addImage(const char* fileimage, bool p1) {
        if (!GJBaseGameLayer::get() || !pusabFix()) return CCTextureCache::addImage(fileimage, p1);
        if (std::string_view(fileimage) == "bigFont.png") {
            return CCTextureCache::addImage("bigFont.png"_spr, p1);
        }
        return CCTextureCache::addImage(fileimage, p1);
    }
};