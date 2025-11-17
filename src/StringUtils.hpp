#pragma once

#include <Geode/Geode.hpp>

struct StringHash {
    using is_transparent = void;
    size_t operator()(std::string_view s) const noexcept {
        return std::hash<std::string_view>{}(s);
    }
    size_t operator()(const std::string& s) const noexcept {
        return std::hash<std::string>{}(s);
    }
};

struct StringEq {
    using is_transparent = void;
    bool operator()(std::string_view a, std::string_view b) const noexcept {
        return a == b;
    }
    bool operator()(const std::string& a, std::string_view b) const noexcept {
        return a == b;
    }
};

