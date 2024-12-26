#pragma once

#include <Geode/Geode.hpp>
#include "../Utils.h"

class Config {
    public:
    bool m_batchNodesEnabled = true;

    void loadPackJsons() {
        for (std::string packPath : Utils::getActivePacks()) {
            std::filesystem::path json = std::filesystem::path{packPath} / "pack.json";
            if (!std::filesystem::exists(json)) continue;

            std::ifstream inputFile(json);

            auto res = matjson::parse(inputFile);
            if (!res.ok()) continue;
            matjson::Value jsonRes = res.unwrap();

            if (jsonRes.contains("happy-textures") && jsonRes["happy-textures"].isObject()) {
                loadConfig(jsonRes["happy-textures"]);
            }
        }
    }

    void loadConfig(matjson::Value config) {
        if (m_batchNodesEnabled) m_batchNodesEnabled = config["batch-nodes"].asBool().unwrapOr(true);
    }

    static Config& get() {
        static Config instance;
        return instance;
    }

};