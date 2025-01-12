#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

struct PackInfo {
    VersionInfo m_textureldr;
    std::string m_id;
    std::string m_name;
    VersionInfo m_version;
    std::vector<std::string> m_authors;
};

class Pack {
protected:
    std::filesystem::path m_path;
    std::filesystem::path m_unzippedPath;
    std::filesystem::path m_resourcesPath;
    std::optional<PackInfo> m_info;
public:
    std::optional<PackInfo> getInfo() {
        return m_info;
    }
};
