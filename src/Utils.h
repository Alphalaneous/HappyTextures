#pragma once

#ifndef HAPPYUTILS_H
#define HAPPYUTILS_H

#include <Geode/Geode.hpp>
using namespace geode::prelude;

namespace Utils {

    inline std::string toLower(std::string s) {        
        for(char &c : s){
            c = tolower(c);
        }
        return s;
    }

    inline bool endsWith(std::string value, std::string ending){
        value = toLower(value);
        ending = toLower(ending);

        return value.ends_with(ending);
    }

    inline bool startsWith(std::string value, std::string start){
        value = toLower(value);
        start = toLower(start);

        return value.starts_with(start);
    }
    inline void updateSprite(CCMenuItemSpriteExtra* button) {

		auto sprite = button->getNormalImage();
		auto size = sprite->getScaledContentSize();
		sprite->setPosition(size / 2);
		sprite->setAnchorPoint({ .5f, .5f });
		button->setContentSize(size);
	}

    inline bool hasNode(CCNode* child, CCNode* node) {
        CCNode* parent = child;
        while(true){
            if(parent){
                if(parent == node){
                    return true;
                }
                parent = parent->getParent();
            }
            else break;
        }
        return false;
    }

    //fix texture loader fallback

    inline CCSprite* getValidSprite(const char* sprName){
        CCSprite* spr = CCSprite::create(sprName);
        if(!spr || spr->getUserObject("geode.texture-loader/fallback")){
            return nullptr;
        }
        return spr;
    }

    //fix texture loader fallback

    inline CCSprite* getValidSpriteFrame(const char* sprName){
        CCSprite* spr = CCSprite::createWithSpriteFrameName(sprName);
        if(!spr || spr->getUserObject("geode.texture-loader/fallback")){
            return nullptr;
        }
        return spr;
    }

    static std::vector<std::string> getActivePacks(){

        gd::vector<gd::string> paths = CCFileUtils::sharedFileUtils()->getSearchPaths();
        std::vector<std::string> packPaths;
        Mod* textureLoader = Loader::get()->getLoadedMod("geode.texture-loader");
        if(textureLoader){
            std::filesystem::path textureLoaderPacks = textureLoader->getConfigDir();
            std::string packDirStr = fmt::format("{}{}", textureLoaderPacks, "\\packs");
            std::filesystem::path packDir = std::filesystem::path(packDirStr);

            for(std::string path : paths){

                std::filesystem::path fpath = std::filesystem::path(path);
                std::filesystem::path pathParent = std::filesystem::path(path);

                while(pathParent.has_parent_path()){

                    if(pathParent == packDir){
                        if(std::find(packPaths.begin(), packPaths.end(), fpath.string()) == packPaths.end()) {
                            packPaths.push_back(fpath.string());
                            break;
                        }
                    }
                    if(pathParent == std::filesystem::current_path().root_path()){
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
#endif