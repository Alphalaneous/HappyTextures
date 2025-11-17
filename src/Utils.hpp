#pragma once

#include <Geode/Geode.hpp>
#include "UIModding.hpp"
#include "Macros.hpp"
#include <geode.texture-loader/include/TextureLoader.hpp>
#include <alphalaneous.alphas_geode_utils/include/Utils.h>
#include <queue>

using namespace geode::prelude;

namespace Utils {

    class BFSNodeTreeCrawler final {
    private:
        std::queue<CCNode*> m_queue;
        std::unordered_set<CCNode*> m_explored;

    public:
        BFSNodeTreeCrawler(CCNode* target) {
            if (!target) return;
            if (auto first = getChild(target, 0)) {
                m_explored.insert(first);
                m_queue.push(first);
            }
        }

        CCNode* next() {
            if (m_queue.empty()) {
                return nullptr;
            }
            auto node = m_queue.front();
            m_queue.pop();
            for (auto sibling : CCArrayExt<CCNode*>(node->getParent()->getChildren())) {
                if (!m_explored.contains(sibling)) {
                    m_explored.insert(sibling);
                    m_queue.push(sibling);
                }
            }
            for (auto child : CCArrayExt<CCNode*>(node->getChildren())) {
                if (!m_explored.contains(child)) {
                    m_explored.insert(child);
                    m_queue.push(child);
                }
            }
            return node;
        }
    };

    class NodeQuery final {
    private:
        enum class Op {
            ImmediateChild,
            DescendantChild,
        };

        std::string m_targetID;
        std::string m_targetClass;
        int m_targetIndex = 0;
        Op m_nextOp;
        std::unique_ptr<NodeQuery> m_next = nullptr;

    public:
        static void setNextOp(std::optional<Op>& nextOp, NodeQuery*& current, std::string& collectedID, std::string& collectedClass) {
            if (nextOp) {
                current->m_next = std::make_unique<NodeQuery>();
                current->m_nextOp = *nextOp;
                current->m_targetID = collectedID;
                current->m_targetClass = collectedClass;
                current = current->m_next.get();

                collectedID.clear();
                collectedClass.clear();
                nextOp = std::nullopt;
            }
        }

        static Result<std::unique_ptr<NodeQuery>> parse(std::string_view query) {
            if (query.empty()) {
                return Err("Query may not be empty");
            }

            auto result = std::make_unique<NodeQuery>();
            NodeQuery* current = result.get();

            size_t i = 0;
            std::string collectedID;
            std::string collectedClass;
            std::optional<Op> nextOp = Op::DescendantChild;
            bool inClassBrackets = false;

            while (i < query.size()) {
                auto c = query.at(i);

                if (inClassBrackets) {
                    if (c == ']') {
                        inClassBrackets = false;

                        auto colonPos = collectedClass.find(':');
                        if (colonPos != std::string::npos) {
                            if (colonPos + 1 < collectedClass.size()) {
                                current->m_targetIndex = numFromString<int>(collectedClass.data() + colonPos + 1).unwrapOr(0);
                            } 
                            collectedClass.resize(colonPos);
                        }
                    }
                    else {
                        collectedClass.push_back(c);
                    }
                }

                else if (c == ' ') {
                    if (!nextOp) {
                        nextOp.emplace(Op::DescendantChild);
                    }
                }
                else if (c == '>') {
                    if (!nextOp || *nextOp == Op::DescendantChild) {
                        nextOp.emplace(Op::ImmediateChild);
                    }
                    else {
                        return Err("Can't have multiple child operators at once (index {})", i);
                    }
                }
                else if (c == '[') {
                    setNextOp(nextOp, current, collectedID, collectedClass);
                    inClassBrackets = true;
                }
                else if (std::isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '_' || c == '/' || c == '.') {
                    setNextOp(nextOp, current, collectedID, collectedClass);
                    collectedID.push_back(c);
                }
                else {
                    return Err("Unexpected character '{}' at index {}", c, i);
                }

                i += 1;
            }

            if (inClassBrackets) {
                return Err("Unclosed class name bracket");
            }
            if (nextOp || (collectedID.empty() && collectedClass.empty())) {
                return Err("Expected node ID or class but got end of query");
            }

            current->m_targetID = collectedID;
            current->m_targetClass = collectedClass;

            return Ok(std::move(result));
        }

        CCNode* match(CCNode* node) const {
            if (!m_targetID.empty() && node->getID() != m_targetID) {
                return nullptr;
            }

            if (!m_targetClass.empty()) {
                if (AlphaUtils::Cocos::getClassName(node, true) != m_targetClass) {
                    return nullptr;
                }

                if (auto parent = node->getParent()) {
                    int matchCount = 0;
                    bool found = false;
                    for (auto c : CCArrayExt<CCNode*>(parent->getChildren())) {
                        if (AlphaUtils::Cocos::getClassName(node, true) == m_targetClass) {
                            if (matchCount == m_targetIndex) {
                                if (c != node) return nullptr;
                                found = true;
                                break;
                            }
                            matchCount++;
                        }
                    }
                    if (!found) return nullptr;
                }
            }

            if (!m_next) {
                return node;
            }
            switch (m_nextOp) {
                case Op::ImmediateChild: {
                    for (auto c : CCArrayExt<CCNode*>(node->getChildren())) {
                        if (auto r = m_next->match(c)) {
                            return r;
                        }
                    }
                } break;

                case Op::DescendantChild: {
                    auto crawler = BFSNodeTreeCrawler(node);
                    while (auto c = crawler.next()) {
                        if (auto r = m_next->match(c)) {
                            return r;
                        }
                    }
                } break;
            }
            return nullptr;
        }

        std::string toString() const {
            auto str = m_targetID.empty() ? "&" : m_targetID;
            if (!m_targetClass.empty()) {
                str += "[" + m_targetClass;
                if (m_targetIndex != 0) {
                    str += ":" + std::to_string(m_targetIndex);
                }
                str += "]";
            }
            if (m_next) {
                switch (m_nextOp) {
                    case Op::ImmediateChild: str += " > "; break;
                    case Op::DescendantChild: str += " "; break;
                }
                str += m_next->toString();
            }
            return str;
        }
    };

    static CCNode* nodeForQuery(CCNode* node, const std::string& queryStr) {
        auto res = NodeQuery::parse(queryStr);
        if (!res) return nullptr;
        
        auto query = std::move(res.unwrap());
        return query->match(node);
    }

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

    static std::string_view trim(std::string_view s) {
        const auto is_space = [](unsigned char c) {
            return c == ' ' || c == '\t' || c == '\n' ||
                c == '\r' || c == '\f' || c == '\v';
        };

        while (!s.empty() && is_space(s.front()))
            s.remove_prefix(1);

        while (!s.empty() && is_space(s.back()))
            s.remove_suffix(1);

        return s;
    }

    static Result<bool> boolFromString(std::string_view str) {
        if (str == "true") return Ok(true);
        if (str == "false") return Ok(false);
        return Err("Invalid bool string");
    };

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
        UIModding::get()->uiCacheHpt.clear();
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

            const auto packStr = utils::string::pathToString(path);
            const size_t baseLen = packStr.length() + 1;

            for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
                if (!entry.is_regular_file()) continue;

                const std::string fullPath = utils::string::pathToString(entry);
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

    static uint8_t clampByte(float v) {
        return static_cast<uint8_t>(std::clamp(std::round(v), 0.f, 255.f));
    }

    static std::vector<std::string> split(std::string_view str, std::string_view delimiter) {
        std::vector<std::string> result;
        size_t start = 0;

        while (start <= str.size()) {
            size_t end = str.find(delimiter, start);
            if (end == std::string_view::npos) {
                end = str.size();
            }
            result.emplace_back(str.substr(start, end - start));
            start = end + delimiter.size();
        }

        return result;
    }

    static Result<std::vector<float>> parseFloatArgs(std::string_view str) {
        auto parts = split(str, ",");
        std::vector<float> values;
        values.reserve(parts.size());
        for (auto& part : parts) {
            auto numRes = numFromString<float>(utils::string::trim(part));
            if (!numRes) return Err("Invalid number in args: " + part);
            values.push_back(numRes.unwrap());
        }
        return Ok(values);
    }

    // HEX
    static Result<ccColor4B> parseHex(std::string_view str) {
        auto hexColorRes = cc4bFromHexString(std::string(str));
        if (!hexColorRes) return Err("Invalid Hex Color");
        return Ok(hexColorRes.unwrap());
    }

    // rgb
    static Result<ccColor4B> parseRGB(std::string_view str) {
        GEODE_UNWRAP_INTO(auto values, parseFloatArgs(str));
        if (values.size() != 3) return Err("rgb requires 3 values");
        return Ok(ccColor4B {
            clampByte(values[0]),
            clampByte(values[1]),
            clampByte(values[2]),
            255
        });
    }

    // rgba
    static Result<ccColor4B> parseRGBA(std::string_view str) {
        GEODE_UNWRAP_INTO(auto values, parseFloatArgs(str));
        if (values.size() != 4) return Err("rgba requires 4 values");
        return Ok(ccColor4B {
            clampByte(values[0]),
            clampByte(values[1]),
            clampByte(values[2]),
            clampByte(values[3] * 255.f)
        });
    }

    // HSL
    static ccColor4B hslToRgb(float h, float s, float l, float a) {
        h = std::fmod(h, 360.f) / 360.f;
        s = std::clamp(s, 0.f, 1.f);
        l = std::clamp(l, 0.f, 1.f);

        auto hue2rgb = [](float p, float q, float t) {
            if (t < 0) t += 1.f;
            if (t > 1) t -= 1.f;
            if (t < 1.f/6.f) return p + (q - p) * 6.f * t;
            if (t < 1.f/2.f) return q;
            if (t < 2.f/3.f) return p + (q - p) * (2.f/3.f - t) * 6.f;
            return p;
        };

        float r, g, b;
        if (s == 0) {
            r = g = b = l;
        } else {
            float q = l < 0.5f ? l * (1.f + s) : l + s - l * s;
            float p = 2.f * l - q;
            r = hue2rgb(p, q, h + 1.f/3.f);
            g = hue2rgb(p, q, h);
            b = hue2rgb(p, q, h - 1.f/3.f);
        }

        return { clampByte(r * 255.f), clampByte(g * 255.f), clampByte(b * 255.f), clampByte(a * 255.f) };
    }

    static Result<ccColor4B> parseHSL(std::string_view str) {
        GEODE_UNWRAP_INTO(auto values, parseFloatArgs(str));
        if (values.size() != 3) return Err("hsl requires 3 values");
        return Ok(hslToRgb(values[0], values[1], values[2], 1.f));
    }

    static Result<ccColor4B> parseHSLA(std::string_view str) {
        GEODE_UNWRAP_INTO(auto values, parseFloatArgs(str));
        if (values.size() != 4) return Err("hsla requires 4 values");
        return Ok(hslToRgb(values[0], values[1], values[2], values[3]));
    }

    // HSV
    static ccColor4B hsvToRgb(float h, float s, float v, float a) {
        h = std::fmod(h, 360.f) / 60.f;
        s = std::clamp(s, 0.f, 1.f);
        v = std::clamp(v, 0.f, 1.f);

        int i = static_cast<int>(std::floor(h)) % 6;
        float f = h - std::floor(h);
        float p = v * (1.f - s);
        float q = v * (1.f - f * s);
        float t = v * (1.f - (1.f - f) * s);

        float r, g, b;
        switch (i) {
            case 0: r = v; g = t; b = p; break;
            case 1: r = q; g = v; b = p; break;
            case 2: r = p; g = v; b = t; break;
            case 3: r = p; g = q; b = v; break;
            case 4: r = t; g = p; b = v; break;
            default: r = v; g = p; b = q; break;
        }

        return { clampByte(r * 255.f), clampByte(g * 255.f), clampByte(b * 255.f), clampByte(a * 255.f) };
    }

    static Result<ccColor4B> parseHSV(std::string_view str) {
        GEODE_UNWRAP_INTO(auto values, parseFloatArgs(str));
        if (values.size() != 3) return Err("hsv requires 3 values");
        return Ok(hsvToRgb(values[0], values[1], values[2], 1.f));
    }

    static Result<ccColor4B> parseHSVA(std::string_view str) {
        GEODE_UNWRAP_INTO(auto values, parseFloatArgs(str));
        if (values.size() != 4) return Err("hsva requires 4 values");
        return Ok(hsvToRgb(values[0], values[1], values[2], values[3]));
    }

    // LAB
    static ccColor4B labToRgb(float L, float a, float b, float alpha) {
        float y = (L + 16.f) / 116.f;
        float x = a / 500.f + y;
        float z = y - b / 200.f;

        auto f = [](float t) {
            float t3 = t * t * t;
            return t3 > 0.008856f ? t3 : (t - 16.f/116.f) / 7.787f;
        };

        x = 95.047f * f(x);
        y = 100.000f * f(y);
        z = 108.883f * f(z);

        float r = x * 0.032406f + y * -0.015372f + z * -0.004986f;
        float g = x * -0.009689f + y * 0.018758f + z * 0.000415f;
        float bl = x * 0.000557f + y * -0.002040f + z * 0.010570f;

        auto gamma = [](float c) {
            return c <= 0.0031308f ? 12.92f * c : 1.055f * std::pow(c, 1.f/2.4f) - 0.055f;
        };

        return { clampByte(gamma(r) * 255.f), clampByte(gamma(g) * 255.f), clampByte(gamma(bl) * 255.f), clampByte(alpha * 255.f) };
    }

    static Result<ccColor4B> parseLAB(std::string_view str) {
        GEODE_UNWRAP_INTO(auto values, parseFloatArgs(str));
        if (values.size() != 3) return Err("lab requires 3 values");
        return Ok(labToRgb(values[0], values[1], values[2], 1.f));
    }

    // LCH
    static Result<ccColor4B> parseLCH(std::string_view str) {
        GEODE_UNWRAP_INTO(auto values, parseFloatArgs(str));
        if (values.size() != 3) return Err("lch requires 3 values");

        float L = values[0];
        float C = values[1];
        float H = values[2] * (M_PI / 180.f);

        float a = C * std::cos(H);
        float b = C * std::sin(H);

        return Ok(labToRgb(L, a, b, 1.f));
    }

    static Result<ccColor4B> parseColorFromString(std::string_view color) {
        auto trimmed = trim(color);

        if (trimmed.starts_with("#")) {
            return parseHex(trimmed);
        }

        auto open = trimmed.find('(');
        auto close = trimmed.find(')');
        if (open == std::string::npos || close == std::string::npos || close <= open) {
            return Err("Invalid color format");
        }

        auto fn = trimmed.substr(0, open);
        auto args = trimmed.substr(open + 1, close - open - 1);

        if (fn == "rgb") return parseRGB(args);
        if (fn == "rgba") return parseRGBA(args);
        if (fn == "hsl") return parseHSL(args);
        if (fn == "hsla") return parseHSLA(args);
        if (fn == "hsv") return parseHSV(args);
        if (fn == "hsva") return parseHSVA(args);
        if (fn == "lab") return parseLAB(args);
        if (fn == "lch") return parseLCH(args);

        return Err("Unknown color function: {}", fn);
    }

    static Result<ccColor3B> parseColor3BFromString(const std::string& color) {
        auto ret = parseColorFromString(color);
        if (!ret) return Err(ret.unwrapErr());
        auto c = ret.unwrap();
        return Ok(ccColor3B{c.r, c.g, c.b});
    }

    static CCActionInterval* getEasingType(const std::string& name, CCActionInterval* action, float rate) {
        CCActionInterval* easingType = nullptr;
        
        if (name == "none")  return action;
        
        typeForEaseCC(EaseInOut);
        typeForEaseCC(EaseIn);
        typeForEaseCC(EaseOut);
        typeForEaseRate(ElasticInOut);
        typeForEaseRate(ElasticIn);
        typeForEaseRate(ElasticOut);
        typeForEase(BounceInOut);
        typeForEase(BounceIn);
        typeForEase(BounceOut);
        typeForEase(ExponentialInOut);
        typeForEase(ExponentialIn);
        typeForEase(ExponentialOut);
        typeForEase(SineInOut);
        typeForEase(SineIn);
        typeForEase(SineOut);
        typeForEase(BackInOut);
        typeForEase(BackIn);
        typeForEase(BackOut);

        return easingType;
    }

    static CCActionInterval* createAction(CCNode* node, const matjson::Value& action) {
        if (!action.contains("type")) return nullptr;

        std::string type = action["type"].asString().unwrapOr("");
        if (type == "Stop") {
            node->stopAllActions();
            return nullptr;
        }

        float duration = action["duration"].asDouble().unwrapOr(0.f);
        float easingRate = action["easing-rate"].asDouble().unwrapOr(0.f);
        std::string easingType = action["easing"].asString().unwrapOr("none");
        int repeatCount = INT32_MAX;
        bool repeat = false;

        if (action.contains("repeat")) {
            const auto& r = action["repeat"];
            if (r.isBool()) repeat = r.asBool().unwrapOr(false);
            if (r.isNumber()) {
                repeat = true;
                repeatCount = r.asInt().unwrapOr(INT32_MAX);
            }
        }

        float x = 0, y = 0, value = 0;
        if (const auto& val = action["value"]; val.isNumber()) {
            value = val.asDouble().unwrapOr(0.f);
            x = value;
            y = value;
        } else if (val.isObject()) {
            x = val["x"].asDouble().unwrapOr(val["width"].asDouble().unwrapOr(0.f));
            y = val["y"].asDouble().unwrapOr(val["height"].asDouble().unwrapOr(0.f));
        }

        CCActionInterval* actionToDo = nullptr;

        if (type == "Sequence") {
            CCArray* sequence = CCArray::create();
            if (action["actions"].isArray()) {
                if (auto arr = action["actions"].asArray().unwrap(); !arr.empty()) {
                    for (auto& a : arr) {
                        if (auto act = createAction(node, a)) {
                            sequence->addObject(act);
                        }
                    }
                }
            }
            if (sequence->count() > 0) actionToDo = CCSequence::create(sequence);
        }

        actionCase(MoveBy, duration, ccp(x, y))
        actionCase(MoveTo, duration, ccp(x, y))
        actionCase(SkewBy, duration, x, y)
        actionCase(SkewTo, duration, x, y)
        actionCase(FadeIn, duration)
        actionCase(FadeOut, duration)
        actionCase(FadeTo, duration, value)
        actionCase(ScaleTo, duration, x, y)
        actionCase(ScaleBy, duration, x, y)
        actionCase(RotateBy, duration, x)
        actionCase(RotateTo, duration, x)

        if (!actionToDo) return nullptr;

        auto finalAction = Utils::getEasingType(easingType, actionToDo, easingRate);
        if (!finalAction) return nullptr;

        return repeat ? CCRepeat::create(finalAction, repeatCount) : finalAction;
    }

    static CCActionInterval* createActionHpt(CCNode* node, const matjson::Value& action) {
        if (!action.contains("type")) return nullptr;

        std::string type = action["type"].asString().unwrapOr("");
        if (type == "Stop") {
            node->stopAllActions();
            return nullptr;
        }

        float duration = action["duration"].asDouble().unwrapOr(0.f);
        float easingRate = action["easing-rate"].asDouble().unwrapOr(0.f);
        std::string easingType = action["easing"].asString().unwrapOr("none");
        int repeatCount = INT32_MAX;
        bool repeat = false;

        if (action.contains("repeat")) {
            const auto& r = action["repeat"];
            if (r.isBool()) repeat = r.asBool().unwrapOr(false);
            if (r.isNumber()) {
                repeat = true;
                repeatCount = r.asInt().unwrapOr(INT32_MAX);
            }
        }

        float x = 0, y = 0, value = 0;
        if (const auto& val = action["value"]; val.isNumber()) {
            value = val.asDouble().unwrapOr(0.f);
            x = value;
            y = value;
        } else if (val.isObject()) {
            x = val["x"].asDouble().unwrapOr(val["width"].asDouble().unwrapOr(0.f));
            y = val["y"].asDouble().unwrapOr(val["height"].asDouble().unwrapOr(0.f));
        }

        CCActionInterval* actionToDo = nullptr;

        if (type == "Sequence") {
            CCArray* sequence = CCArray::create();
            if (action["actions"].isArray()) {
                if (auto arr = action["actions"].asArray().unwrap(); !arr.empty()) {
                    for (auto& a : arr) {
                        if (auto act = createAction(node, a)) {
                            sequence->addObject(act);
                        }
                    }
                }
            }
            if (sequence->count() > 0) actionToDo = CCSequence::create(sequence);
        }

        actionCase(MoveBy, duration, ccp(x, y))
        actionCase(MoveTo, duration, ccp(x, y))
        actionCase(SkewBy, duration, x, y)
        actionCase(SkewTo, duration, x, y)
        actionCase(FadeIn, duration)
        actionCase(FadeOut, duration)
        actionCase(FadeTo, duration, value)
        actionCase(ScaleTo, duration, x, y)
        actionCase(ScaleBy, duration, x, y)
        actionCase(RotateBy, duration, x)
        actionCase(RotateTo, duration, x)

        if (!actionToDo) return nullptr;

        auto finalAction = Utils::getEasingType(easingType, actionToDo, easingRate);
        if (!finalAction) return nullptr;

        return repeat ? CCRepeat::create(finalAction, repeatCount) : finalAction;
    }

    static unsigned int stringToBlendingMode(const std::string& value) {
        static const std::unordered_map<std::string, unsigned int> strBlend = {
            {"GL_ZERO", GL_ZERO},
            {"GL_ONE", GL_ONE},
            {"GL_SRC_COLOR", GL_SRC_COLOR},
            {"GL_ONE_MINUS_SRC_COLOR", GL_ONE_MINUS_SRC_COLOR},
            {"GL_DST_COLOR", GL_DST_COLOR},
            {"GL_ONE_MINUS_DST_COLOR", GL_ONE_MINUS_DST_COLOR},
            {"GL_SRC_ALPHA", GL_SRC_ALPHA},
            {"GL_ONE_MINUS_SRC_ALPHA", GL_ONE_MINUS_SRC_ALPHA},
            {"GL_DST_ALPHA", GL_DST_ALPHA},
            {"GL_ONE_MINUS_DST_ALPHA", GL_ONE_MINUS_DST_ALPHA},
            {"GL_SRC_ALPHA_SATURATE", GL_SRC_ALPHA_SATURATE},
            {"GL_CONSTANT_COLOR", GL_CONSTANT_COLOR},
            {"GL_ONE_MINUS_CONSTANT_COLOR", GL_ONE_MINUS_CONSTANT_COLOR},
            {"GL_CONSTANT_ALPHA", GL_CONSTANT_ALPHA},
            {"GL_ONE_MINUS_CONSTANT_ALPHA", GL_ONE_MINUS_CONSTANT_ALPHA}
        };
        auto it = strBlend.find(value);
        return it != strBlend.end() ? it->second : -1;
    }

    static std::vector<std::string> generateValidSprites(const std::string& path, const std::vector<std::string>& spriteList) {
        std::vector<std::string> validSprites;

        if (!path.empty()) {
            for (const auto& pack : Utils::getActivePacks()) {
                std::filesystem::path sprPath = pack.resourcesPath / path;
                if (!std::filesystem::is_directory(sprPath)) continue;

                for (const auto& entry : std::filesystem::directory_iterator(sprPath)) {
                    const auto& file = entry.path();
                    const std::string name = file.filename().stem().string();

                    if (utils::string::endsWith(name, "-hd") || utils::string::endsWith(name, "-uhd"))
                        continue;

                    const std::string sprName = fmt::format("{}\\{}", path, file.filename().string());
                    if (Utils::getValidSprite(sprName.c_str()))
                        validSprites.push_back(sprName);
                }
            }
        }

        for (const auto& v : spriteList) {
            if (Utils::getValidSprite(v.c_str()))
                validSprites.push_back(v);
        }

        return validSprites;
    }

    static void openURLSafe(const std::string& url) {
        auto lower = utils::string::toLower(url);
        utils::string::trimIP(lower);
        if (utils::string::startsWith(lower, "http://") || utils::string::startsWith(lower, "https://")) {
            utils::web::openLinkInBrowser(lower);
        }
    }
}