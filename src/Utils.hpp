#pragma once

#include <Geode/Geode.hpp>
#include "UIModding.hpp"
#include "Macros.hpp"
#include <geode.texture-loader/include/TextureLoader.hpp>
#include <alphalaneous.alphas_geode_utils/include/Utils.h>

using namespace geode::prelude;

namespace Utils {

    static void updateSprite(CCMenuItemSpriteExtra* button) {
        auto* sprite = button->getNormalImage();
        if (!sprite) return;

        const auto& size = sprite->getScaledContentSize();
        sprite->setPosition(size / 2);
        sprite->setAnchorPoint({0.5f, 0.5f});
        button->setContentSize(size);
    }

    template <typename Layer, typename = std::enable_if_t<std::is_pointer_v<Layer>>>
    static Layer getLayer() {
        return AlphaUtils::Cocos::getLayer<Layer>().value_or(nullptr);
    }

    static int getValidStat(const std::string& key) {
        return GameStatsManager::sharedState()->getStat(key.c_str());
    }

    static GJGameLevel* getLevel() {
        GJGameLevel* level = nullptr;
        if (GJBaseGameLayer* gjbgl = GJBaseGameLayer::get()) {
            level = gjbgl->m_level;
        }
        if (LevelInfoLayer* lil = Utils::getLayer<LevelInfoLayer*>()) {
            level = lil->m_level;
        }
        return level;
    }

    static bool hasNode(CCNode* child, CCNode* node) {
        return AlphaUtils::Cocos::hasNode(child, node);
    }

    //fix texture loader fallback

    static CCSprite* getValidSprite(const char* sprName) {
        return AlphaUtils::Cocos::getSprite(sprName).value_or(nullptr);
    }

    static CCSprite* getValidSpriteFrame(const char* sprName) {
        return AlphaUtils::Cocos::getSpriteByFrameName(sprName).value_or(nullptr);
    }

    static void setColorIfExists(CCRGBAProtocol* node, const std::string& colorId) {
        if (!node) return;

        if (const auto dataOpt = UIModding::get()->getColors(colorId)) {
            const ColorData& data = *dataOpt;
            node->setColor(data.color);
            node->setOpacity(data.alpha);
        }
    }

    static std::string getSpriteName(CCSprite* sprite) {
        
        if (auto texture = sprite->getTexture()) {
            for (const auto& [key, frame] : CCDictionaryExt<std::string, CCSpriteFrame*>(CCSpriteFrameCache::sharedSpriteFrameCache()->m_pSpriteFrames)) {
                if (frame->getTexture() == texture && frame->getRect() == sprite->getTextureRect()) return key;
            }
            for (const auto& [key, obj] : CCDictionaryExt<std::string, CCTexture2D*>(CCTextureCache::sharedTextureCache()->m_pTextures)) {
                if (obj == texture) return key;
            }
        }
        return "";
    }

    static int getRandomNumber(int lower, int upper) {
        return AlphaUtils::Misc::getRandomNumber(lower, upper);
    }

    static void clearCaches() {
        UIModding::get()->uiCache.clear();
        UIModding::get()->colorCache.clear();
        UIModding::get()->filenameCache.clear();
        UIModding::get()->textureToNameMap.clear();
    }

    static std::optional<geode::texture_loader::Pack> getPackByID(const std::string& id) {
        for (const auto& pack : geode::texture_loader::getAppliedPacks()) {
            if (pack.id == id) return pack;
        }
        for (const auto& pack : geode::texture_loader::getAvailablePacks()) {
            if (pack.id == id) return pack;
        }
        return std::nullopt;
    }

    static const std::vector<texture_loader::Pack> getActivePacks() {
        if (Mod* textureLoader = Loader::get()->getLoadedMod("geode.texture-loader")) {
            return geode::texture_loader::getAppliedPacks();
        }

        return {};
    }

    static void qualityToNormal(std::string& str) {
        size_t dotPos = str.rfind('.');
        if (dotPos == std::string::npos || dotPos == 0 || dotPos == str.size() - 1)
            return;

        size_t prefixEnd = dotPos;

        if (prefixEnd >= 4 && str.compare(prefixEnd - 4, 4, "-uhd") == 0) {
            prefixEnd -= 4;
        } else if (prefixEnd >= 3 && str.compare(prefixEnd - 3, 3, "-hd") == 0) {
            prefixEnd -= 3;
        }

        if (prefixEnd < dotPos) {
            str.erase(prefixEnd, dotPos - prefixEnd);
        }
    }

    static void reloadFileNames() {
        auto& filenameCache = UIModding::get()->filenameCache;

        for (const auto& pack : Utils::getActivePacks()) {
            auto path = pack.resourcesPath;
            if (!std::filesystem::is_directory(path)) continue;

            const auto packStr = path.string();
            const size_t baseLen = packStr.length() + 1;

            for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
                if (!entry.is_regular_file()) continue;

                const std::string fullPath = entry.path().string();
                if (fullPath.length() <= baseLen) continue;

                std::string subStr = fullPath.substr(baseLen);
                qualityToNormal(subStr);

                geode::utils::string::replaceIP(subStr, "\\", "/");
                filenameCache[std::move(subStr)] = true;
            }
        }
    }

    static bool spriteExistsInPacks(const std::string& fileName) {
        return UIModding::get()->filenameCache[fileName];
    }

    static CCNode* getChildByTypeName(CCNode* node, int index, const std::string& name) {
        return AlphaUtils::Cocos::getChildByClassName(node, name, index).value_or(nullptr);
    }

}