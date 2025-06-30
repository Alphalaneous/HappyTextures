#pragma once

#include <Geode/Geode.hpp>
#include "UIModding.hpp"
#include <random>
#include "Macros.hpp"
#include <geode.texture-loader/include/TextureLoader.hpp>

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
        auto* scene = CCDirector::sharedDirector()->getRunningScene();
        if (!scene) return nullptr;

        if (auto* trans = typeinfo_cast<CCTransitionScene*>(scene)) {
            scene = public_cast(trans, m_pInScene);
            if (!scene) return nullptr;
        }

        return scene->getChildByType<Layer>(0);
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
        for (CCNode* current = child; current != nullptr; current = current->getParent()) {
            if (current == node) {
                return true;
            }
        }
        return false;
    }

    
    //fix texture loader fallback

    static CCSprite* getValidSprite(const char* sprName) {
        CCSprite* spr = CCSprite::create(sprName);
        if (!spr || spr->getUserObject("geode.texture-loader/fallback")) {
            return nullptr;
        }
        return spr;
    }

    //fix texture loader fallback

    static CCSprite* getValidSpriteFrame(const char* sprName) {
        CCSprite* spr = CCSprite::createWithSpriteFrameName(sprName);
        if (!spr || spr->getUserObject("geode.texture-loader/fallback")) {
            return nullptr;
        }
        return spr;
    }

    static std::string nameForClass(const std::string& className) {
        size_t lastSpace = className.rfind(' ');
        size_t start = (lastSpace == std::string::npos) ? 0 : lastSpace + 1;

        size_t lastColon = className.rfind("::");
        size_t nameStart = (lastColon != std::string::npos && lastColon >= start)
            ? lastColon + 2
            : start;

        return className.substr(nameStart);
    }

    static std::string getNodeName(CCObject* node) {
    #ifdef GEODE_IS_WINDOWS
        return nameForClass(typeid(*node).name() + 6);
    #else 
        std::string ret;

        int status = 0;
        auto demangle = abi::__cxa_demangle(typeid(*node).name(), 0, 0, &status);
        if (status == 0) {
            ret = demangle;
        }
        free(demangle);

        return nameForClass(ret);
    #endif
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
        if (lower > upper) {
            std::swap(lower, upper);
        }

        static std::random_device rd; 
        static std::mt19937 gen(rd()); 

        std::uniform_int_distribution<> dist(lower, upper);
        return dist(gen);
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

        if (str.compare(prefixEnd - 4, 4, "-uhd") == 0) {
            prefixEnd -= 4;
        } else if (str.compare(prefixEnd - 3, 3, "-hd") == 0) {
            prefixEnd -= 3;
        }

        if (prefixEnd < dotPos) {
            str.erase(prefixEnd, dotPos - prefixEnd);
            dotPos = prefixEnd;
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
        if (!node || node->getChildrenCount() == 0) return nullptr;

        size_t indexCounter = 0;
        const size_t childrenCount = node->getChildrenCount();

        bool isNegativeIndex = (index < 0);
        if (isNegativeIndex) {
            index = -index - 1; 
        }

        for (size_t i = (isNegativeIndex ? childrenCount - 1 : 0); 
            isNegativeIndex ? i >= 0 : i < childrenCount; 
            isNegativeIndex ? --i : ++i) {

            CCNode* idxNode = static_cast<CCNode*>(node->getChildren()->objectAtIndex(i));
            if (getNodeName(idxNode) == name) {
                if (indexCounter == index) {
                    return idxNode;
                }
                ++indexCounter;
            }

            if (isNegativeIndex && i == 0) break;
        }

        return nullptr;
    }

}