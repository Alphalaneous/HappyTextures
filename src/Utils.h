#pragma once

#include <Geode/Geode.hpp>
#include "UIModding.h"
#include <random>

using namespace geode::prelude;

namespace Utils {

    static void updateSprite(CCMenuItemSpriteExtra* button) {

		auto sprite = button->getNormalImage();
		auto size = sprite->getScaledContentSize();
		sprite->setPosition(size / 2);
		sprite->setAnchorPoint({ .5f, .5f });
		button->setContentSize(size);
	}

    template <typename T>
    static T getLayer() {
        if (CCScene* scene = CCDirector::get()->m_pRunningScene) {
            for (CCNode* node : CCArrayExt<CCNode*>(scene->getChildren())) {
                if (T layer = typeinfo_cast<T>(node)) {
                    return layer;
                }
            }
        }
        return nullptr;
    }

    static int getValidStat(std::string key) {
        std::string val = GameStatsManager::sharedState()->m_playerStats->valueForKey(key)->m_sString;

        if (val.empty()) val = "0";
        Result<int> result = geode::utils::numFromString<int>(val);
        if (result.isOk()) return result.unwrap();
        return -1;
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
    
    static std::string strReplace(std::string subject, std::string search, std::string replace) {
        size_t pos = 0;
        while ((pos = subject.find(search, pos)) != std::string::npos) {
            subject.replace(pos, search.length(), replace);
            pos += replace.length();
        }
        return subject;
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

    static std::string nameForClass(std::string className) {
        std::vector<std::string> strSplit = utils::string::split(className, " ");
        std::string part0 = strSplit[strSplit.size()-1];
        std::vector<std::string> strColonSplit = utils::string::split(part0, "::");
        std::string part1 = strColonSplit[strColonSplit.size()-1];
        return part1;
    }

    static std::string getNodeName(CCObject* node) {
    #ifdef GEODE_IS_WINDOWS
        return nameForClass(typeid(*node).name() + 6);
    #else 
        {
            std::string ret;

            int status = 0;
            auto demangle = abi::__cxa_demangle(typeid(*node).name(), 0, 0, &status);
            if (status == 0) {
                ret = demangle;
            }
            free(demangle);

            return nameForClass(ret);
        }
    #endif
    }

    

    static void setColorIfExists(CCRGBAProtocol* node, std::string colorId) {
        if (!node) return;
        std::optional<ColorData> dataOpt = UIModding::get()->getColors(colorId);
        if (dataOpt.has_value()) {
            ColorData data = dataOpt.value();
            node->setColor(data.color);
            node->setOpacity(data.alpha);
        }
    }

    static std::string getSpriteName(CCSprite* sprite) {
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

    static int getRandomNumber(int lower, int upper) {
        if (lower > upper) {
            std::swap(lower, upper);
        }

        static std::random_device rd; 
        static std::mt19937 gen(rd()); 

        std::uniform_int_distribution<> dist(lower, upper);
        return dist(gen);
    }

    static std::vector<std::string> getActivePacks() {

        gd::vector<gd::string> paths = CCFileUtils::sharedFileUtils()->getSearchPaths();
        std::vector<std::string> packPaths;
        Mod* textureLoader = Loader::get()->getLoadedMod("geode.texture-loader");
        if (textureLoader) {
            for (matjson::Value value : textureLoader->getSavedValue<std::vector<matjson::Value>>("applied")) {
                if (value.isObject() && value.contains("path") && value["path"].isString()) {
                    packPaths.push_back(value["path"].asString().unwrap() + "\\");
                }
            }
        }
        std::string resourcesDir = fmt::format("{}{}", CCFileUtils::sharedFileUtils()->getWritablePath2(), "Resources\\");
        packPaths.push_back(resourcesDir);

        return packPaths;
    }

    static CCNode* getChildByTypeName(CCNode* node, int index, std::string name) {
        size_t indexCounter = 0;

        if (!node || node->getChildrenCount() == 0) return nullptr;
        // start from end for negative index
        if (index < 0) {
            index = -index - 1;
            for (size_t i = node->getChildrenCount() - 1; i >= 0; i--) {
                CCNode* idxNode = static_cast<CCNode*>(node->getChildren()->objectAtIndex(i));
                std::string className = getNodeName(idxNode);
                if (className == name) {
                    if (indexCounter == index) {
                        return idxNode;
                    }
                    ++indexCounter;
                }
                if (i == 0) break;
            }
        }
        else {
            for (size_t i = 0; i < node->getChildrenCount(); i++) {
                CCNode* idxNode = static_cast<CCNode*>(node->getChildren()->objectAtIndex(i));
                std::string className = getNodeName(idxNode);
                if (className == name) {
                    if (indexCounter == index) {
                        return idxNode;
                    }
                    ++indexCounter;
                }
            }
        }

        return nullptr;
    }
}