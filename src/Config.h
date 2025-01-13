#pragma once

#include <Geode/Geode.hpp>
#include "Utils.h"
#include <geode.texture-loader/include/TextureLoader.hpp>

class Config {
    protected:
    static Config* instance;
    public:
    std::unordered_map<std::string, std::vector<matjson::Value>> m_packSettings;

    std::optional<matjson::Value> loadIntoJson(geode::texture_loader::Pack pack) {
        std::filesystem::path json = pack.resourcesPath / "pack.json";
        if (!std::filesystem::exists(json)) return std::nullopt;

        std::ifstream inputFile(json);

        auto res = matjson::parse(inputFile);
        if (!res.ok()) return std::nullopt;
        matjson::Value jsonRes = res.unwrap();

        return jsonRes;
    }

    void loadPackJsons() {

        Mod* textureLoader = Loader::get()->getLoadedMod("geode.texture-loader");
        if (textureLoader) {
            if (textureLoader->getVersion() >= VersionInfo{1, 7, 0}) {
                for (geode::texture_loader::Pack pack : geode::texture_loader::getAppliedPacks()) {
                    std::optional<matjson::Value> json = loadIntoJson(pack);
                    if (json.has_value()) {
                        matjson::Value jsonRes = json.value();
                        loadConfig(pack, jsonRes["happy-textures"]);
                    }
                }
                for (geode::texture_loader::Pack pack : geode::texture_loader::getAvailablePacks()) {
                    std::optional<matjson::Value> json = loadIntoJson(pack);
                    if (json.has_value()) {
                        matjson::Value jsonRes = json.value();
                        loadSettings(pack, jsonRes["happy-textures"]);
                    }
                }
            }
        }
    }

    bool hasSettings(std::string modID) {
        return m_packSettings.contains(modID);
    }

    void loadSettings(geode::texture_loader::Pack pack, matjson::Value config) {
        if (config.contains("settings") && config["settings"].isArray()) {
            m_packSettings[pack.id] = config["settings"].asArray().unwrap();
        }
    }

    void loadConfig(geode::texture_loader::Pack pack, matjson::Value config) {
        
        loadSettings(pack, config);
    }

    static Config* get() {
        static Config* instance = nullptr;

        if (!instance) {
            instance = new Config();
        };
        return instance;
    }
};