#pragma once

#include <Geode/Geode.hpp>
#include "UIModding.h"
using namespace geode::prelude;

namespace Utils {

    inline void updateSprite(CCMenuItemSpriteExtra* button) {

		auto sprite = button->getNormalImage();
		auto size = sprite->getScaledContentSize();
		sprite->setPosition(size / 2);
		sprite->setAnchorPoint({ .5f, .5f });
		button->setContentSize(size);
	}

    inline bool hasNode(CCNode* child, CCNode* node) {
        CCNode* parent = child;
        while (true) {
            if (parent) {
                if (parent == node) {
                    return true;
                }
                parent = parent->getParent();
            }
            else break;
        }
        return false;
    }
    
    inline std::string strReplace(std::string subject, std::string search, std::string replace) {
        size_t pos = 0;
        while ((pos = subject.find(search, pos)) != std::string::npos) {
            subject.replace(pos, search.length(), replace);
            pos += replace.length();
        }
        return subject;
    }

    //fix texture loader fallback

    inline CCSprite* getValidSprite(const char* sprName) {
        CCSprite* spr = CCSprite::create(sprName);
        if (!spr || spr->getUserObject("geode.texture-loader/fallback")) {
            return nullptr;
        }
        return spr;
    }

    //fix texture loader fallback

    inline CCSprite* getValidSpriteFrame(const char* sprName) {
        CCSprite* spr = CCSprite::createWithSpriteFrameName(sprName);
        if (!spr || spr->getUserObject("geode.texture-loader/fallback")) {
            return nullptr;
        }
        return spr;
    }

    inline void setColorIfExists(CCRGBAProtocol* node, std::string colorId) {
        if (!node) return;
        std::optional<ColorData> dataOpt = UIModding::get()->getColors(colorId);
        if (dataOpt.has_value()) {
            ColorData data = dataOpt.value();
            node->setColor(data.color);
            node->setOpacity(data.alpha);
        }
    }

    inline std::string getSpriteName(CCSprite* sprite) {
        if (auto texture = sprite->getTexture()) {
            for (auto [key, frame] : CCDictionaryExt<std::string, CCSpriteFrame*>(CCSpriteFrameCache::sharedSpriteFrameCache()->m_pSpriteFrames)) {
                if (frame->getTexture() == texture && frame->getRect() == sprite->getTextureRect()) return key;
            }
            for (auto [key, obj] : CCDictionaryExt<std::string, CCTexture2D*>(CCTextureCache::sharedTextureCache()->m_pTextures)) {
                if (obj == texture) return key;
            }
        }
        return "";
    }

    static std::vector<std::string> getActivePacks() {

        gd::vector<gd::string> paths = CCFileUtils::sharedFileUtils()->getSearchPaths();
        std::vector<std::string> packPaths;
        Mod* textureLoader = Loader::get()->getLoadedMod("geode.texture-loader");
        if (textureLoader) {
            std::filesystem::path textureLoaderPacks = textureLoader->getConfigDir();
            std::string packDirStr = fmt::format("{}{}", textureLoaderPacks, "\\packs");
            std::filesystem::path packDir = std::filesystem::path(packDirStr);

            for (std::string path : paths) {

                std::filesystem::path fpath = std::filesystem::path(path);
                std::filesystem::path pathParent = std::filesystem::path(path);

                while (pathParent.has_parent_path()) {

                    if (pathParent == packDir) {
                        if (std::find(packPaths.begin(), packPaths.end(), fpath.string()) == packPaths.end()) {
                            packPaths.push_back(fpath.string());
                            break;
                        }
                    }
                    if (pathParent == std::filesystem::current_path().root_path()) {
                        break;
                    }
                    pathParent = pathParent.parent_path();
                }
            }
        }
        std::string resourcesDir = fmt::format("{}{}", CCFileUtils::sharedFileUtils()->getWritablePath2(), "Resources\\");
        packPaths.push_back(resourcesDir);

        return packPaths;
    }
}