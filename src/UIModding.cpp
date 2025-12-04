#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include "FileWatcher.hpp"
#include "Utils.hpp"
#include "nodes/CCLabelBMFont.hpp"
#include "nodes/CCMenuItemSpriteExtra.hpp"
#include "nodes/FLAlertLayer.hpp"
#include "nodes/CCNode.hpp"
#include "UIModding.hpp"
#include "DataNode.hpp"
#include "alerts/CustomAlert.hpp"
#include "Callbacks.hpp"
#include "Macros.hpp"
#include "Config.hpp"
#include "HPTParser.hpp"
#include "HPTCCNode.hpp"
#include <alphalaneous.pages_api/include/PagesAPI.h>

using namespace geode::prelude;

UIModding* UIModding::instance = nullptr;

$execute {
    UIModding::get()->doModify = Mod::get()->getSettingValue<bool>("ui-modifications");
    HPTParser::get().setupParser();
}

void UIModding::updateColors(CCNode* node, const std::string& name) {
    if (auto* bg = typeinfo_cast<CCScale9Sprite*>(node->getChildByIDRecursive(name))) {
        if (auto dataOpt = getColors(name)) {
            const auto& data = dataOpt.value();
            bg->setColor(data.color);
            bg->setOpacity(data.alpha);
        }
    }
}

std::optional<ColorData> UIModding::getColors(const std::string& name) {
    if (colorCache.contains(name)) {
        const auto& cached = colorCache[name];
        return cached.hasColor ? std::optional<ColorData>(cached) : std::nullopt;
    }

    constexpr const char* path = "ui/colors.json";
    unsigned long fileSize = 0;
    auto* buffer = CCFileUtils::sharedFileUtils()->getFileData(path, "rb", &fileSize);

    std::optional<ColorData> result = std::nullopt;

    if (buffer && fileSize != 0) {
        std::string json(reinterpret_cast<char*>(buffer), fileSize);
        if (auto parsed = matjson::parse(json); parsed.isOk()) {
            auto& root = parsed.unwrap();
            if (root.isObject() && root.contains(name)) {
                auto& val = root[name];
                if (val.isObject() && val.contains("r") && val.contains("g") && val.contains("b")) {
                    if (val["r"].isNumber() && val["g"].isNumber() && val["b"].isNumber()) {
                        auto r = static_cast<unsigned char>(val["r"].asInt().unwrapOr(0));
                        auto g = static_cast<unsigned char>(val["g"].asInt().unwrapOr(0));
                        auto b = static_cast<unsigned char>(val["b"].asInt().unwrapOr(0));
                        auto a = static_cast<unsigned char>(val.contains("a") && val["a"].isNumber()
                                 ? val["a"].asInt().unwrapOr(255)
                                 : 255);

                        ColorData data = {ccColor3B{r, g, b}, a, true};
                        colorCache[name] = data;
                        result = data;
                    }
                }
                else if (val.isString()) {
                    std::string str = val.asString().unwrapOr("");
                    auto color = Utils::parseColorFromString(str).unwrapOr(ccColor4B{255, 0, 255, 255});
                    ColorData data = {ccColor3B{color.r, color.g, color.b}, color.a, true};
                    colorCache[name] = data;
                    result = data;
                }
            }
        }
    }

    if (!result.has_value()) {
        colorCache[name] = {ccColor3B{0, 0, 0}, 0, false};
    }

    delete[] buffer;
    return result;
}

void UIModding::recursiveModify(CCNode* node, const matjson::Value& elements) {
    auto* children = node->getChildren();

    for (auto* child : CCArrayExt<CCNode*>(children)) {
        auto myChild = static_cast<MyCCNode*>(child);
        std::string id = myChild->getBaseID().empty() ? child->getID() : myChild->getBaseID();

        if (elements.contains(id)) {
            auto nodeValue = elements[id];
            if (nodeValue.isObject()) {
                nodeValue["_pack-name"] = elements["_pack-name"].asString().unwrapOr("missing");
                handleModifications(child, nodeValue);
            }
        }
    }
}

void UIModding::queryModify(CCNode* node, const matjson::Value& elements) {
    for (const auto& [k, v] : elements) {
        std::string query = utils::string::replace(k, "{id}", elements["_pack-name"].asString().unwrapOr("missing"));
        CCNode* nodeRet = Utils::nodeForQuery(node, query);
        handleModifications(nodeRet, v);
    }
}

void UIModding::runAction(CCNode* node, const matjson::Value& attributes) {
    if (!attributes.contains("actions")) return;

    const auto& actionsValue = attributes["actions"];
    if (!actionsValue.isArray()) return;

    auto actionArray = CCArray::create();
    auto actionValues = actionsValue.asArray();

    for (const auto& action : actionValues.unwrap()) {
        if (!action.isObject()) continue;

        if (auto actionInterval = Utils::createAction(node, action)) {
            actionArray->addObject(actionInterval);
        }
    }

    if (actionArray->count() > 0) {
        node->runAction(CCSpawn::create(actionArray));
    }
}

void UIModding::runScrollToTop(CCNode* node, const matjson::Value& attributes) {
    if (!attributes.contains("scroll-to-top")) return;

    if (attributes["scroll-to-top"].asBool().unwrapOr(false)) {
        if (auto scrollLayer = typeinfo_cast<geode::ScrollLayer*>(node)) {
            scrollLayer->scrollToTop();
        }
    }
}

void UIModding::runCallback(CCNode* node, const matjson::Value& attributes) {
    if (!attributes.contains("callback"))
        return;

    const auto& callbackValue = attributes["callback"];
    if (!callbackValue.contains("class") || !callbackValue.contains("method"))
        return;

    std::string className = callbackValue["class"].asString().unwrapOr("");
    std::string methodName = callbackValue["method"].asString().unwrapOr("");

    auto& callbacks = Callbacks::get()->m_callbacks;
    if (auto classIt = callbacks.find(className); classIt != callbacks.end()) {
        auto& methodMap = classIt->second;
        if (auto methodIt = methodMap.find(methodName); methodIt != methodMap.end()) {
            auto& [target, selector] = methodIt->second;
            (target->*selector)(Callbacks::get()->getDummyButton());
        }
    }
}

void UIModding::setLayout(CCNode* node, const matjson::Value& attributes) {
    if (!attributes.contains("layout")) return;

    if (matjson::Value layoutValue = attributes["layout"]; layoutValue.isObject()) {
        Layout* layout;

        if (node->getLayout()) {
            if (layoutValue.contains("remove")) {
                matjson::Value removeValue = layoutValue["remove"];
                if (removeValue.isBool() && removeValue.asBool().unwrapOr(false)) {
                    node->setLayout(nullptr);
                }
            }
            else {
                layout = node->getLayout();
            }
        } else {
            layout = AxisLayout::create(Axis::Row);
            node->setLayout(layout);
        }

        if (auto simpleAxisLayout = typeinfo_cast<SimpleAxisLayout*>(layout)) {
            if (layoutValue.contains("axis")) {
                if (matjson::Value axisValue = layoutValue["axis"]; axisValue.isString()) {
                    std::string axis = axisValue.asString().unwrapOr("");
                    if (axis == "row") {
                        simpleAxisLayout->setAxis(Axis::Row);
                    } else if (axis == "column") {
                        simpleAxisLayout->setAxis(Axis::Column);
                    }
                }
            }
            if (layoutValue.contains("flip-axis")) {
                if (matjson::Value flipAxisValue = layoutValue["flip-axis"]; flipAxisValue.isBool()) {
                    bool flipAxis = flipAxisValue.asBool().unwrapOr(false);
                    simpleAxisLayout->setMainAxisDirection(flipAxis ? AxisDirection::RightToLeft : AxisDirection::LeftToRight);
                }
            }
            if (layoutValue.contains("ignore-invisible")) {
                if (matjson::Value ignoreInvisibleValue = layoutValue["ignore-invisible"]; ignoreInvisibleValue.isBool()) {
                    bool ignoreInvisible = ignoreInvisibleValue.asBool().unwrapOr(false);
                    simpleAxisLayout->ignoreInvisibleChildren(ignoreInvisible);
                }
            }
            if (layoutValue.contains("flip-cross-axis")) {
                if (matjson::Value flipCrossAxisValue = layoutValue["flip-cross-axis"]; flipCrossAxisValue.isBool()) {
                    bool flipCrossAxis = flipCrossAxisValue.asBool().unwrapOr(false);
                    simpleAxisLayout->setCrossAxisDirection(flipCrossAxis ? AxisDirection::RightToLeft : AxisDirection::LeftToRight);
                }
            }
            if (layoutValue.contains("gap")) {
                if (matjson::Value gapValue = layoutValue["gap"]; gapValue.isNumber()) {
                    float gap = gapValue.asDouble().unwrapOr(0);
                    simpleAxisLayout->setGap(gap);
                }
            }
            if (layoutValue.contains("axis-alignment")) {
                if (matjson::Value axisAlignmentValue = layoutValue["axis-alignment"]; axisAlignmentValue.isString()) {
                    std::string axisAlignmentStr = axisAlignmentValue.asString().unwrapOr("");
                    MainAxisAlignment axisAlignment = getSimpleMainAxisAlignment(axisAlignmentStr);
                    simpleAxisLayout->setMainAxisAlignment(axisAlignment);
                }
            }
            if (layoutValue.contains("cross-axis-alignment")) {
                if (matjson::Value crossAxisAlignmentValue = layoutValue["cross-axis-alignment"]; crossAxisAlignmentValue.isString()) {
                    std::string crossAxisAlignmentStr = crossAxisAlignmentValue.asString().unwrapOr("");
                    CrossAxisAlignment axisAlignment = getSimpleCrossAxisAlignment(crossAxisAlignmentStr);
                    simpleAxisLayout->setCrossAxisAlignment(axisAlignment);
                }
            }
        }

        if (auto axisLayout = typeinfo_cast<AxisLayout*>(layout)) {
            if (layoutValue.contains("axis")) {
                if (matjson::Value axisValue = layoutValue["axis"]; axisValue.isString()) {
                    std::string axis = axisValue.asString().unwrapOr("");
                    if (axis == "row") {
                        axisLayout->setAxis(Axis::Row);
                    } else if (axis == "column") {
                        axisLayout->setAxis(Axis::Column);
                    }
                }
            }
            if (layoutValue.contains("flip-axis")) {
                if (matjson::Value flipAxisValue = layoutValue["flip-axis"]; flipAxisValue.isBool()) {
                    bool flipAxis = flipAxisValue.asBool().unwrapOr(false);
                    axisLayout->setAxisReverse(flipAxis);
                }
            }
            if (layoutValue.contains("ignore-invisible")) {
                if (matjson::Value ignoreInvisibleValue = layoutValue["ignore-invisible"]; ignoreInvisibleValue.isBool()) {
                    bool ignoreInvisible = ignoreInvisibleValue.asBool().unwrapOr(false);
                    axisLayout->ignoreInvisibleChildren(ignoreInvisible);
                }
            }
            if (layoutValue.contains("flip-cross-axis")) {
                if (matjson::Value flipCrossAxisValue = layoutValue["flip-cross-axis"]; flipCrossAxisValue.isBool()) {
                    bool flipCrossAxis = flipCrossAxisValue.asBool().unwrapOr(false);
                    axisLayout->setCrossAxisReverse(flipCrossAxis);
                }
            }
            if (layoutValue.contains("auto-scale")) {
                if (matjson::Value autoScaleValue = layoutValue["auto-scale"]; autoScaleValue.isBool()) {
                    bool autoScale = autoScaleValue.asBool().unwrapOr(false);
                    axisLayout->setAutoScale(autoScale);
                }
            }
            if (layoutValue.contains("grow-cross-axis")) {
                if (matjson::Value growCrossAxisValue = layoutValue["grow-cross-axis"]; growCrossAxisValue.isBool()) {
                    bool growCrossAxis = growCrossAxisValue.asBool().unwrapOr(false);
                    axisLayout->setGrowCrossAxis(growCrossAxis);
                }
            }
            if (layoutValue.contains("allow-cross-axis-overflow")) {
                if (matjson::Value allowCrossAxisOverflowValue = layoutValue["allow-cross-axis-overflow"]; allowCrossAxisOverflowValue.isBool()) {
                    bool allowCrossAxisOverflow = allowCrossAxisOverflowValue.asBool().unwrapOr(false);
                    axisLayout->setCrossAxisOverflow(allowCrossAxisOverflow);
                }
            }
            if (layoutValue.contains("gap")) {
                if (matjson::Value gapValue = layoutValue["gap"]; gapValue.isNumber()) {
                    float gap = gapValue.asDouble().unwrapOr(0);
                    axisLayout->setGap(gap);
                }
            }
            if (layoutValue.contains("axis-alignment")) {
                if (matjson::Value axisAlignmentValue = layoutValue["axis-alignment"]; axisAlignmentValue.isString()) {
                    std::string axisAlignmentStr = axisAlignmentValue.asString().unwrapOr("");
                    AxisAlignment axisAlignment = getAxisAlignment(axisAlignmentStr);
                    axisLayout->setAxisAlignment(axisAlignment);
                }
            }
            if (layoutValue.contains("cross-axis-alignment")) {
                if (matjson::Value crossAxisAlignmentValue = layoutValue["cross-axis-alignment"]; crossAxisAlignmentValue.isString()) {
                    std::string crossAxisAlignmentStr = crossAxisAlignmentValue.asString().unwrapOr("");
                    AxisAlignment axisAlignment = getAxisAlignment(crossAxisAlignmentStr);
                    axisLayout->setCrossAxisAlignment(axisAlignment);
                }
            }
            if (layoutValue.contains("cross-axis-line-alignment")) {
                if (matjson::Value crossAxisLineAlignmentValue = layoutValue["cross-axis-line-alignment"]; crossAxisLineAlignmentValue.isString()) {
                    std::string crossAxisLineAlignmentStr = crossAxisLineAlignmentValue.asString().unwrapOr("");
                    AxisAlignment axisAlignment = getAxisAlignment(crossAxisLineAlignmentStr);
                    axisLayout->setCrossAxisLineAlignment(axisAlignment);
                }
            }
        }

        node->updateLayout();
    }
}

std::string UIModding::getSound(std::string_view sound) {
    const auto& paths = CCFileUtils::sharedFileUtils()->getSearchPaths();

    for (const auto& path : paths) {
        std::filesystem::path soundPath = std::filesystem::path(path) / sound;
        if (std::filesystem::exists(soundPath)) {
            return soundPath.string();
        }
    }

    return "";
}

void UIModding::playSound(CCNode* node, const matjson::Value& attributes) {
    if (!attributes.contains("sound")) return;

    std::string sound = attributes["sound"].asString().unwrapOr("");

    if (!sound.empty()) {
        FMODAudioEngine::sharedEngine()->m_backgroundMusicChannel->setPaused(false);
        FMODAudioEngine::sharedEngine()->m_globalChannel->setPaused(false);
        FMODAudioEngine::sharedEngine()->m_system->update();

        std::string soundPath = getSound(sound);
        if (!soundPath.empty()) {
            FMODAudioEngine::sharedEngine()->playEffectAdvanced(
                soundPath, 1, 0, 1, 1, true, false, 0, 0, 0, 0, false, 0, false, true, 0, 0, 0, 0
            );
        }
    }
}

void UIModding::openLink(CCNode* node, const matjson::Value& attributes) {
    if (!attributes.contains("link")) return;

    matjson::Value linkVal = attributes["link"];

    if (linkVal.isString()) {
        Utils::openURLSafe(linkVal.asString().unwrapOr(""));
    }
    else if (linkVal.isObject()) {
        if (linkVal.contains("type") && linkVal.contains("id")) {
            matjson::Value typeVal = linkVal["type"];
            matjson::Value idVal = linkVal["id"];

            if (typeVal.isString() && idVal.isNumber()) {
                std::string type = typeVal.asString().unwrapOr("");
                int id = idVal.asInt().unwrapOr(0);

                if (type == "profile") {
                    ProfilePage::create(id, false)->show();
                }
                else if (type == "level") {
                    auto searchObject = GJSearchObject::create(SearchType::Type19, fmt::format("{}&gameVersion=22", id));
                    auto scene = LevelBrowserLayer::scene(searchObject);
                    CCDirector::get()->pushScene(CCTransitionFade::create(0.5f, scene));
                }
            }
        }
    }
}

void UIModding::setShow(CCNode* node, const matjson::Value& attributes) {
    if (!attributes.contains("show")) return;

    if (matjson::Value showVal = attributes["show"]; showVal.isBool()) {
        bool show = showVal.asBool().unwrapOr(false);
        if (FLAlertLayer* alert = typeinfo_cast<FLAlertLayer*>(node)) {
            if (show) alert->show();
            else alert->keyBackClicked();
        }
    }
}

void UIModding::setZOrder(CCNode* node, const matjson::Value& attributes) {
    if (!attributes.contains("z-order")) return;

    if (matjson::Value zOrderVal = attributes["z-order"]; zOrderVal.isNumber()) {
        int zOrder = zOrderVal.asInt().unwrapOr(0);
        node->setZOrder(zOrder);
    }
}

void UIModding::setBlending(CCNode* node, const matjson::Value& attributes) {
    if (!attributes.contains("blending")) return;

    matjson::Value blendingVal = attributes["blending"];

    if (!blendingVal.isObject()) return;

    if (!blendingVal.contains("source") || !blendingVal.contains("destination")) return;

    matjson::Value sourceVal = blendingVal["source"];
    matjson::Value destinationVal = blendingVal["destination"];

    if (!sourceVal.isString() || !destinationVal.isString()) return;

    std::string source = sourceVal.asString().unwrapOr("");
    std::string destination = destinationVal.asString().unwrapOr("");

    unsigned int src = Utils::stringToBlendingMode(source);
    unsigned int dst = Utils::stringToBlendingMode(destination);

    if (src != -1 && dst != -1) {
        if (CCBlendProtocol* blendable = typeinfo_cast<CCBlendProtocol*>(node)) {
            blendable->setBlendFunc({src, dst});
        }
    }
}

void UIModding::setFlip(CCNode* node, const matjson::Value& attributes) {
    if (!attributes.contains("flip")) return;

    auto flipVal = attributes["flip"];
    if (!flipVal.isObject()) return;

    bool flipX = false;
    bool flipY = false;

    if (flipVal.contains("x")) {
        auto xVal = flipVal["x"];
        if (xVal.isBool()) flipX = xVal.asBool().unwrapOr(false);
    }
    if (flipVal.contains("y")) {
        auto yVal = flipVal["y"];
        if (yVal.isBool()) flipY = yVal.asBool().unwrapOr(false);
    }

    if (auto sprite = typeinfo_cast<CCSprite*>(node)) {
        sprite->setFlipX(flipX);
        sprite->setFlipY(flipY);
    }
}

void UIModding::setFont(CCNode* node, const matjson::Value& attributes) {
    if (!attributes.contains("font")) return;

    auto fontVal = attributes["font"];
    if (!fontVal.isString()) return;

    std::string font = fontVal.asString().unwrapOr("");
    CCLabelBMFont* textObject = nullptr;

    if (auto textNode = typeinfo_cast<CCLabelBMFont*>(node)) {
        textObject = textNode;
    }
    else if (auto buttonNode = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
        if (auto searchButton = node->getChildByType<SearchButton>(0)) {
            textObject = searchButton->getChildByType<CCLabelBMFont>(0);
        } else if (auto buttonSprite = node->getChildByType<ButtonSprite>(0)) {
            textObject = buttonSprite->getChildByType<CCLabelBMFont>(0);
        } else {
            textObject = node->getChildByType<CCLabelBMFont>(0);
        }
    }

    if (textObject && utils::string::endsWith(font, ".fnt")) {
        auto myTextObject = static_cast<MyCCLabelBMFont*>(textObject);
        auto& fields = myTextObject->m_fields;

        if (FNTConfigLoadFile(font.c_str())) {
            textObject->setFntFile(font.c_str());

            if (fields->m_isLimited) {
                textObject->limitLabelWidth(fields->m_limitWidth, fields->m_limitDefaultScale, fields->m_limitMinScale);
            }
        }
    }
}

void UIModding::setPosition(CCNode* node, const matjson::Value& attributes) {
    if (!attributes.contains("position")) return;

    auto position = attributes["position"];
    if (!position.isObject()) return;

    float x = 0;
    float y = 0;

    if (position.contains("x") && position.contains("y")) {
        auto xVal = position["x"];
        auto yVal = position["y"];
        if (xVal.isNumber() && yVal.isNumber()) {
            x = xVal.asDouble().unwrapOr(0);
            y = yVal.asDouble().unwrapOr(0);
        }
    }

    if (position.contains("anchor")) {
        auto anchorValue = position["anchor"];
        if (anchorValue.isObject() || anchorValue.isString()) {
            CCNode* parent = node->getParent();
            CCSize nodeSize = parent ? parent->getContentSize() : CCDirector::sharedDirector()->getWinSize();

            if (!anchorValue.isString() && anchorValue.contains("relative")) {
                auto relativeValue = anchorValue["relative"];
                if (relativeValue.isString()) {
                    std::string relative = relativeValue.asString().unwrapOr("");
                    if (relative == "screen") nodeSize = CCDirector::sharedDirector()->getWinSize();
                    else if (relative == "parent") nodeSize = parent->getContentSize();
                }
            }

            if (anchorValue.contains("to") || anchorValue.isString()) {
                matjson::Value anchorToValue = anchorValue.isString() ? anchorValue : anchorValue["to"];
                if (anchorToValue.isString()) {
                    std::string anchorTo = anchorToValue.asString().unwrapOr("");
                    if (anchorTo == "top-left") {
                        y += nodeSize.height;
                    } else if (anchorTo == "top-center") {
                        x += nodeSize.width / 2;
                        y += nodeSize.height;
                    } else if (anchorTo == "top-right") {
                        x += nodeSize.width;
                        y += nodeSize.height;
                    } else if (anchorTo == "center-left") {
                        y += nodeSize.height / 2;
                    } else if (anchorTo == "center") {
                        x += nodeSize.width / 2;
                        y += nodeSize.height / 2;
                    } else if (anchorTo == "center-right") {
                        x += nodeSize.width;
                        y += nodeSize.height / 2;
                    } else if (anchorTo == "bottom-center") {
                        x += nodeSize.width / 2;
                    } else if (anchorTo == "bottom-right") {
                        x += nodeSize.width;
                    } else if (anchorTo == "self") {
                        x += node->getPosition().x;
                        y += node->getPosition().y;
                    }
                }
            }
        }
    }

    node->setPosition({x, y});
}

void UIModding::setColor(CCNode* node, const matjson::Value& attributes) {
    if (!attributes.contains("color")) return;

    auto color = attributes["color"];

    if (color.isObject() && color.contains("r") && color.contains("g") && color.contains("b")) {
        auto r = color["r"].asInt().unwrapOr(0);
        auto g = color["g"].asInt().unwrapOr(0);
        auto b = color["b"].asInt().unwrapOr(0);
        ccColor3B newColor = {static_cast<unsigned char>(r), static_cast<unsigned char>(g), static_cast<unsigned char>(b)};

        if (auto node1 = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
            node1->setColor(newColor);
            if (auto buttonNode = node1->getChildByType<ButtonSprite>(0)) {
                buttonNode->setColor(newColor);
            }
        } else if (auto node1 = typeinfo_cast<CCRGBAProtocol*>(node)) {
            node1->setColor(newColor);
        }
    } 
    else if (color.isString()) {
        std::string colorStr = color.asString().unwrapOr("");

        if (colorStr == "reset") {
            if (auto node1 = static_cast<EventCCMenuItemSpriteExtra*>(typeinfo_cast<CCMenuItemSpriteExtra*>(node))) {
                auto originalColor = node1->m_fields->originalColor;
                node1->setColor(originalColor);
                if (auto buttonNode = node1->getChildByType<ButtonSprite>(0)) {
                    buttonNode->setColor(originalColor);
                }
            }
        }
        else {
            auto color = Utils::parseColorFromString(colorStr).unwrapOr(ccColor4B{255, 0, 255, 255});
            auto color3B = ccColor3B{color.r, color.g, color.b};
            auto alpha = color.a;

            if (auto node1 = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
                node1->setColor(color3B);
                node1->setOpacity(alpha);
                if (auto buttonNode = node1->getChildByType<ButtonSprite>(0)) {
                    buttonNode->setColor(color3B);
                    buttonNode->setOpacity(alpha);
                }
            }
            if (auto node1 = typeinfo_cast<CCRGBAProtocol*>(node)) {
                node1->setColor(color3B);
                node1->setOpacity(alpha);
            }
        }
    }
}

void UIModding::removeChild(CCNode* node, const matjson::Value& attributes) {
    if (!attributes.contains("remove")) return;

    if (auto removeVal = attributes["remove"]; removeVal.isBool()) {
        bool remove = removeVal.asBool().unwrapOr(false);
        if (remove) removalQueue.push_back(node);
    }
}

void UIModding::setScaleMult(CCNode* node, const matjson::Value& attributes) {
    if (!attributes.contains("scale-multiplier")) return;

    if (auto mulVal = attributes["scale-multiplier"]; mulVal.isNumber()) {
        float multiplier = mulVal.asDouble().unwrapOr(0);
        if (auto button = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
            button->m_scaleMultiplier = multiplier;
        }
    }
}

void UIModding::setScaleBase(CCNode* node, const matjson::Value& attributes) {
    if (!attributes.contains("base-scale")) return;

    if (auto baseVal = attributes["base-scale"]; baseVal.isNumber()) {
        float base = baseVal.asDouble().unwrapOr(0);
        if (auto button = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
            button->m_baseScale = base;
        }
    }
}

void UIModding::setDisablePages(CCNode* node, const matjson::Value& attributes) {
    if (!attributes.contains("disable-pages")) return;

    if (auto pagesVal = attributes["disable-pages"]; pagesVal.isBool()) {
        if (!pagesVal.asBool().unwrap()) {
            PagesAPI::enablePages(typeinfo_cast<CCMenu*>(node), false);
        }
    }
}

void UIModding::setText(CCNode* node, const matjson::Value& attributes) {
    if (!attributes.contains("text")) return;

    if (auto textVal = attributes["text"]; textVal.isString()) {
        std::string text = textVal.asString().unwrapOr("");

        CCLabelBMFont* textObject = nullptr;

        if (auto textNode = typeinfo_cast<CCLabelBMFont*>(node)) {
            textObject = textNode;
        } 
        else if (auto buttonNode = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
            if (auto searchButton = buttonNode->getChildByType<SearchButton>(0)) {
                textObject = searchButton->getChildByType<CCLabelBMFont>(0);
            } 
            else if (auto buttonSprite = buttonNode->getChildByType<ButtonSprite>(0)) {
                textObject = buttonSprite->getChildByType<CCLabelBMFont>(0);
            }
            else {
                textObject = buttonNode->getChildByType<CCLabelBMFont>(0);
            }
        }

        if (textObject) {
            MyCCLabelBMFont* myTextObject = static_cast<MyCCLabelBMFont*>(textObject);
            myTextObject->setHappyTexturesModified();
            textObject->setString(text.c_str());

            auto textFields = myTextObject->m_fields.self();

            if (textFields->m_isLimited) {
                textObject->limitLabelWidth(
                    textFields->m_limitWidth,
                    textFields->m_limitDefaultScale,
                    textFields->m_limitMinScale
                );
            }
        }
    }
}

std::vector<std::string> UIModding::generateValidSprites(const std::string& path, const matjson::Value& spriteList) {
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

    if (spriteList.isArray()) {
        for (const auto& v : spriteList.asArray().unwrap()) {
            if (!v.isString()) continue;

            const std::string& spriteName = v.asString().unwrap();
            if (Utils::getValidSprite(spriteName.c_str()))
                validSprites.push_back(spriteName);
        }
    }

    return validSprites;
}

void UIModding::setSprite(CCNode* node, const matjson::Value& attributes) {
    if (!attributes.contains("sprite") && !attributes.contains("sprite-frame")) return;

    matjson::Value sprite = attributes["sprite"];
    if (attributes.contains("sprite-frame")) sprite = attributes["sprite-frame"];
    std::string spriteName;
    CCSprite* spr = nullptr;

    if (sprite.isObject()) {
        matjson::Value randomObject = sprite["random"];
        std::string mode = randomObject["mode"].asString().unwrapOr("load");
        std::string id = randomObject["id"].asString().unwrapOr("");
        std::string path = randomObject["path"].asString().unwrapOr("");
        matjson::Value spriteList = randomObject["sprites"];

        if (mode == "load" || id.empty()) {
            std::vector<std::string> validSprites = generateValidSprites(path, spriteList);
            if (!validSprites.empty()) spriteName = validSprites.at(Utils::getRandomNumber(0, validSprites.size()-1));
        }
        else if (mode == "session") {
            if (!randomSprites.contains(id)) {
                std::vector<std::string> validSprites = generateValidSprites(path, spriteList);
                if (!validSprites.empty()) randomSprites[id] = validSprites.at(Utils::getRandomNumber(0, validSprites.size()-1));
            }
            spriteName = randomSprites[id];
        }
    }

    if (sprite.isString()) {
        spriteName = sprite.asString().unwrapOr("");
    }

    spr = Utils::getValidSpriteFrame(spriteName.c_str());
    if (!spr) spr = Utils::getValidSprite(spriteName.c_str());
    if (!spr) return;

    if (auto spriteNode = typeinfo_cast<CCSprite*>(node)) {
        spriteNode->setTexture(spr->getTexture());
        spriteNode->setTextureRect(spr->getTextureRect(), spr->isTextureRectRotated(), spr->getContentSize());
        spriteNode->setTextureAtlas(spr->getTextureAtlas());
    }

    if (auto buttonNode = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
        if (attributes.contains("button-info")) {
            matjson::Value infoVal = attributes["button-info"];
            if (infoVal.isObject() && infoVal.contains("type")) {
                std::string type = infoVal["type"].asString().unwrapOr("");

                if (type == "labeled") {
                    std::string caption, font = "bigFont.fnt", texture = "GJ_button_01.png";
                    int width = 30;
                    float height = 30, scale = 1;
                    bool absolute = false;

                    setSpriteVar(caption, text, String, "");
                    setSpriteVar(font, font, String, "");
                    setSpriteVar(texture, sprite, String, "");
                    setSpriteVarNum(width, width, Int, 0);
                    setSpriteVarNum(height, height, Double, 0);
                    setSpriteVarNum(scale, scale, Double, 0);
                    setSpriteVar(absolute, absolute, Bool, false);

                    if (CCSprite* sprite = Utils::getValidSprite(texture.c_str())) {
                        if (FNTConfigLoadFile(font.c_str())) {
                            auto buttonSprite = ButtonSprite::create(caption.c_str(), width, absolute, font.c_str(), texture.c_str(), height, scale);
                            buttonNode->setNormalImage(buttonSprite);
                            Utils::updateSprite(buttonNode);
                        }
                    }
                }
                else if (type == "sprite") {
                    buttonNode->setNormalImage(spr);
                    Utils::updateSprite(buttonNode);
                }
            }
        }

        if (auto buttonSprite = node->getChildByType<ButtonSprite>(0)) {
            buttonSprite->updateBGImage(spriteName.c_str());
        }
        else if (auto childSprite = node->getChildByType<CCSprite>(0)) {
            childSprite->setTexture(spr->getTexture());
            childSprite->setTextureAtlas(spr->getTextureAtlas());
            childSprite->setTextureRect(spr->getTextureRect(), spr->isTextureRectRotated(), spr->getContentSize());
            childSprite->setContentSize(spr->getContentSize());
            buttonNode->setContentSize(spr->getContentSize());
            childSprite->setPosition(buttonNode->getContentSize() / 2);
        }
    }
}

void UIModding::setOpacity(CCNode* node, const matjson::Value& attributes) {
    if (!attributes.contains("opacity")) return;

    auto opacity = attributes["opacity"];

    if (opacity.isNumber()) {
        auto opacityNum = static_cast<GLubyte>(opacity.asInt().unwrapOr(0));

        if (auto node1 = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
            node1->setOpacity(opacityNum);
            if (auto node2 = node1->getChildByType<ButtonSprite>(0)) {
                node2->setOpacity(opacityNum);
            }
        }

        if (auto nodeRGBA = typeinfo_cast<CCNodeRGBA*>(node)) {
            nodeRGBA->setOpacity(opacityNum);
        }
        if (auto layerRGBA = typeinfo_cast<CCLayerRGBA*>(node)) {
            layerRGBA->setOpacity(opacityNum);
        }
    }

    if (opacity.isString() && opacity.asString().unwrapOr("") == "reset") {
        if (auto node1 = static_cast<EventCCMenuItemSpriteExtra*>(typeinfo_cast<CCMenuItemSpriteExtra*>(node))) {
            auto original = node1->m_fields->originalOpacity;
            node1->setOpacity(original);
            if (auto node2 = node1->getChildByType<ButtonSprite>(0)) {
                node2->setOpacity(original);
            }
        }
    }
}

void UIModding::setVisible(CCNode* node, const matjson::Value& attributes) {
    if (!attributes.contains("visible")) return;

    if (auto visible = attributes["visible"]; visible.isBool()) {
        node->setVisible(visible.asBool().unwrapOr(false));
    }
}

void UIModding::updateLayout(CCNode* node, const matjson::Value& attributes) {
    auto update = attributes["update-layout"];
    if (update.isBool() && update.asBool().unwrapOr(false)) {
        if (auto parent = node->getParent()) parent->updateLayout();
        return;
    }
    if (update.isString()) {
        std::string updateStr = update.asString().unwrapOr("self");
        if (updateStr == "self") {
            node->updateLayout();
        } else if (updateStr == "parent") {
            if (auto parent = node->getParent()) parent->updateLayout();
        }
    }
}

void UIModding::setIgnoreAnchorPos(CCNode* node, const matjson::Value& attributes) {
    if (!attributes.contains("ignore-anchor-pos")) return;

    if (auto ignore = attributes["ignore-anchor-pos"]; ignore.isBool()) {
        node->ignoreAnchorPointForPosition(ignore.asBool().unwrapOr(false));
    }
}

void UIModding::setScale(CCNode* node, const matjson::Value& attributes) {
    if (!attributes.contains("scale")) return;

    auto scale = attributes["scale"];

    if (scale.isNumber()) {
        node->setScale(scale.asDouble().unwrapOr(0.0f));
        return;
    }

    if (!scale.isObject()) return;

    if (auto xVal = scale["x"]; xVal.isNumber()) {
        node->setScaleX(xVal.asDouble().unwrapOr(0.0f));
    }
    if (auto yVal = scale["y"]; yVal.isNumber()) {
        node->setScaleY(yVal.asDouble().unwrapOr(0.0f));
    }

    if (auto fit = scale["fit"]; fit.isObject()) {
        std::string fitRelative = fit["relative"].asString().unwrapOrDefault();
        float scaleMultiplier = fit["multiplier"].asDouble().unwrapOr(1.f);
        CCSize targetSize = {-1, -1};
        CCSize screenSize = CCDirector::get()->getWinSize();
        CCSize parentSize = targetSize;
        if (auto parent = node->getParent()) {
            parentSize = parent->getContentSize();
        }
        if (fitRelative == "screen") {
            targetSize = screenSize;
        }
        if (fitRelative == "screen-width") {
            targetSize.width = screenSize.width;
        }
        if (fitRelative == "screen-height") {
            targetSize.height = screenSize.height;
        }
        if (fitRelative == "parent") {
            targetSize = parentSize;
        }
        if (fitRelative == "parent-width") {
            targetSize.width = parentSize.width;
        }
        if (fitRelative == "parent-height") {
            targetSize.height = parentSize.height;
        }
        if (fitRelative == "size") {
            if (auto size = fit["size"]; fit.isObject()) {
                if (auto widthVal = size["width"]; widthVal.isNumber()) {
                    targetSize.width = widthVal.asDouble().unwrapOr(-1.f);
                }
                if (auto heightVal = size["height"]; heightVal.isNumber()) {
                    targetSize.height = heightVal.asDouble().unwrapOr(-1.f);
                }
            }
        }

        CCSize contentSize = node->getContentSize();

        float largestSide = -1;
        float largestContentSide = -1;

        if (targetSize.width != -1 && targetSize.height != -1) {
            if (targetSize.width > targetSize.height) {
                largestSide = targetSize.width;
                largestContentSide = contentSize.width;
            }
            else {
                largestSide = targetSize.height;
                largestContentSide = contentSize.height;
            }
        }
        else if (targetSize.width != -1 ) {
            largestSide = targetSize.width;
            largestContentSide = contentSize.width;
        }
        else if (targetSize.height != -1 ) {
            largestSide = targetSize.height;
            largestContentSide = contentSize.height;
        }

        if (largestSide != -1) {
            float scale = largestSide / largestContentSide;
            node->setScale(scale * scaleMultiplier);
        }
        return;
    }
}

void UIModding::setRotation(CCNode* node, const matjson::Value& attributes) {
    if (!attributes.contains("rotation")) return;

    auto rotation = attributes["rotation"];

    if (rotation.isNumber()) {
        node->setRotation(rotation.asDouble().unwrapOr(0.0f));
        return;
    }

    if (!rotation.isObject()) return;

    if (auto xVal = rotation["x"]; xVal.isNumber()) {
        node->setRotationX(xVal.asDouble().unwrapOr(0.0f));
    }
    if (auto yVal = rotation["y"]; yVal.isNumber()) {
        node->setRotationY(yVal.asDouble().unwrapOr(0.0f));
    }
}

void UIModding::setSkew(CCNode* node, const matjson::Value& attributes) {
    if (!attributes.contains("skew")) return;

    auto skew = attributes["skew"];
    if (!skew.isObject()) return;

    if (auto xVal = skew["x"]; xVal.isNumber()) {
        node->setSkewX(xVal.asDouble().unwrapOr(0.0f));
    }

    if (auto yVal = skew["y"]; yVal.isNumber()) {
        node->setSkewY(yVal.asDouble().unwrapOr(0.0f));
    }
}

void UIModding::setAnchorPoint(CCNode* node, const matjson::Value& attributes) {
    if (attributes.contains("anchor-point")) {
        matjson::Value anchorPoint = attributes["anchor-point"];
        
        if (anchorPoint.isObject()) {
            matjson::Value xVal = anchorPoint["x"];
            matjson::Value yVal = anchorPoint["y"];

            if (xVal.isNumber() && yVal.isNumber()) {
                float x = xVal.asDouble().unwrapOr(0.0f);
                float y = yVal.asDouble().unwrapOr(0.0f);

                node->setAnchorPoint({x, y});
            }
        }
    }
}

void UIModding::setContentSize(CCNode* node, const matjson::Value& attributes) {
    if (attributes.contains("content-size")) {
        matjson::Value contentSize = attributes["content-size"];
        
        if (contentSize.isObject()) {
            matjson::Value widthVal = contentSize["width"];
            matjson::Value heightVal = contentSize["height"];

            if (widthVal.isNumber() && heightVal.isNumber()) {
                float width = widthVal.asDouble().unwrapOr(0.0f);
                float height = heightVal.asDouble().unwrapOr(0.0f);

                node->setContentSize({width, height});
            }
        }
    }
}

void UIModding::evaluateIf(CCNode* node, const matjson::Value& ifArray) {
    if (!node || !ifArray.isArray()) return;

    for (matjson::Value v : ifArray.asArray().unwrap()) {
        if (v.contains("statement")) {
            if (v["statement"].isString()) {
                std::string statement = v["statement"].asString().unwrapOrDefault();
                auto vars = LabelValues::getValueMap("");
                auto nodeVars = LabelValues::getNodeMap(node);
                vars.insert(nodeVars.begin(), nodeVars.end());

                auto result = rift::evaluate(statement, vars);
                bool evaluated = result.unwrapOr("false").toBoolean();

                if (evaluated) {
                    handleModifications(node, v);
                }
            }
        }
    }
}

void UIModding::handleAttributes(CCNode* node, const matjson::Value& attributes) {
    nodesFor(setDisablePages);
    nodesFor(setSprite);
    nodesFor(setScale);
    nodesFor(setRotation);
    nodesFor(setSkew);
    nodesFor(setAnchorPoint);
    nodesFor(setContentSize);
    nodesFor(setVisible);
    nodesFor(setIgnoreAnchorPos);
    nodesFor(setColor);
    nodesFor(setOpacity);
    nodesFor(setPosition);
    nodesFor(setText);
    nodesFor(setScaleMult);
    nodesFor(setScaleBase);
    nodesFor(setZOrder);
    nodesFor(setFont);
    nodesFor(setFlip);
    nodesFor(setBlending);
    nodesFor(setShow);
    nodesFor(removeChild);
    nodesFor(setLayout);
    nodesFor(updateLayout);
    nodesFor(runScrollToTop);
    nodesFor(runAction);
    nodesFor(playSound);
    nodesFor(openLink);
    nodesFor(runCallback);
}

void UIModding::handleEvent(CCNode* node, const matjson::Value& eventVal) {
    forEvent(on-click, OnClick);
    forEvent(on-release, OnRelease);
    forEvent(on-activate, OnActivate);
    forEvent(on-hover, OnHover);
    forEvent(on-exit, OnExit);
}

void UIModding::handleSchedule(CCNode* node, const matjson::Value& scheduleVal) {
    static_cast<MyCCNode*>(node)->scheduleAttribute(scheduleVal);
}

void UIModding::handleParent(CCNode* node, const matjson::Value& parentVal) {
    if (CCNode* parent = node->getParent()) {
        handleModifications(parent, parentVal);
    }
}

void UIModding::handleRoot(CCNode* node, const matjson::Value& rootVal) {
    CCNode* parentResult = node;
    while (parentResult->getParent()) {
        parentResult = parentResult->getParent();
    }
    handleModifications(parentResult, rootVal);
}

void UIModding::handleChildren(CCNode* node, matjson::Value& childrenVal) {
    if (childrenVal.contains("node") && childrenVal["node"].isObject()) {
        childrenVal["node"]["_pack-name"] = childrenVal["_pack-name"];
        recursiveModify(node, childrenVal["node"]);
    }
    if (childrenVal.contains("query") && childrenVal["query"].isObject()) {
        childrenVal["query"]["_pack-name"] = childrenVal["_pack-name"];
        queryModify(node, childrenVal["query"]);
    }
    if (childrenVal.contains("index") && childrenVal["index"].isArray()) {
        handleChildByIndex(node, childrenVal["index"], childrenVal["_pack-name"].asString().unwrapOr("missing"));
    }
    if (childrenVal.contains("all") && childrenVal["all"].isObject()) {
        childrenVal["all"]["_pack-name"] = childrenVal["_pack-name"];
        handleAllChildren(node, childrenVal["all"]);
    }
    if (childrenVal.contains("new") && childrenVal["new"].isArray()) {
        handleNewChildren(node, childrenVal["new"], childrenVal["_pack-name"].asString().unwrapOr("missing"));
    }
    if (childrenVal.contains("move") && childrenVal["move"].isArray()) {
        handleMoveChild(node, childrenVal["move"], childrenVal["_pack-name"].asString().unwrapOr("missing"));
    }
}

void UIModding::handleMoveChild(CCNode* node, const matjson::Value& moveChildrenVal, const std::string& packName) {
    if (moveChildrenVal.isArray()) {
        for (const auto& value : moveChildrenVal.asArray().unwrap()) {
            if (value.isObject()) {
                if (auto nodeVal = value["node"]; nodeVal.isString()) {
                    std::string queryNode = utils::string::replace(nodeVal.asString().unwrapOrDefault(), "{id}", packName);

                    CCNode* nodeTo = Utils::nodeForQuery(node, queryNode);
                    if (!nodeTo) return;
                    moveQueue[nodeTo] = [this, node, nodeTo, value, packName] {
                        if (auto moveVal = value["parent"]; moveVal.isString()) {
                            std::string queryParent = utils::string::replace(moveVal.asString().unwrapOrDefault(), "{id}", packName);
                            CCNode* moveTo = Utils::nodeForQuery(node, queryParent);
                            if (moveTo) {
                                nodeTo->removeFromParentAndCleanup(false);
                                moveTo->addChild(nodeTo);
                                handleModifications(nodeTo, value);
                            }
                        }
                    };
                }
            }
        }
    }
}

void UIModding::handleNewChildren(CCNode* node, matjson::Value& newChildrenVal, const std::string& packName) {
    geode::Result<std::vector<matjson::Value>&> nodeChildrenArray = newChildrenVal.asArray();
    if (nodeChildrenArray.isOk()) {
        for (matjson::Value& value : nodeChildrenArray.unwrap()) {
            if (value.isObject()) {
                value["_pack-name"] = packName;
                createAndModifyNewChild(node, value);
            }
        }
    }
}

void UIModding::handleAllChildren(CCNode* node, const matjson::Value& allChildrenVal) {
    CCArray* children = node->getChildren();
    for (CCNode* node : CCArrayExt<CCNode*>(children)) {
        handleModifications(node, allChildrenVal);
    }
}

void UIModding::handleChildByIndex(CCNode* node, matjson::Value& indexChildrenVal, const std::string& packName) {
    geode::Result<std::vector<matjson::Value>&> nodeChildrenArray = indexChildrenVal.asArray();
    if (nodeChildrenArray.isOk()) {
        for (matjson::Value value : nodeChildrenArray.unwrap()) {
            if (value.isObject()) {
                value["_pack-name"] = packName;
                modifyChildByIndex(node, value);
            }
        }
    }
}

void UIModding::modifyChildByIndex(CCNode* node, const matjson::Value& value) {
    int index = 0;
    std::string type = "CCNode";
    if (value.contains("index")) {
        matjson::Value indexVal = value["index"];
        if (indexVal.isNumber()) {
            index = indexVal.asInt().unwrapOr(0);
        }
    }
    if (value.contains("type")) {
        matjson::Value typeVal = value["type"];
        if (typeVal.isString()) {
            type = typeVal.asString().unwrapOr("");
        }
    }
    UIModding::get()->handleModifications(Utils::getChildByTypeName(node, index, type), value);
}

void UIModding::createAndModifyNewChild(CCNode* node, const matjson::Value& newChildVal) {
    int index = 0;
    std::string type = "Node";
    
    if (newChildVal.contains("type") && newChildVal.contains("id")) {
        matjson::Value typeVal = newChildVal["type"];
        if (typeVal.isString()) {
            type = typeVal.asString().unwrapOr("");

            CCNode* newNode = nullptr;

            if (type == "CCSprite") {
                matjson::Value attributesVal = newChildVal["attributes"];
                std::string sprType = "sprite";
                if (attributesVal.contains("sprite-frame")) sprType = "sprite-frame";
                if ((attributesVal.contains(sprType) && attributesVal[sprType].isString())) {
                    std::string spriteName = attributesVal[sprType].asString().unwrapOr("");
                    newNode = Utils::getValidSpriteFrame(spriteName.c_str());
                    if (!newNode) newNode = Utils::getValidSprite(spriteName.c_str());
                    if (!newNode) newNode = CCSprite::create();
                }
            } else if (type == "CCNode") {
                newNode = CCNode::create();
            } else if (type == "CCLabelBMFont") {
                newNode = CCLabelBMFont::create("", "chatFont.fnt");
            } else if (type == "CCMenu") {
                newNode = CCMenu::create();
            } else if (type == "CCLayerColor") {
                newNode = CCLayerColor::create(ccColor4B{0, 0, 0, 0});
            } else if (type == "CCLayer") {
                newNode = CCLayer::create();
            } else if (type == "CCMenuItemSpriteExtra") {
                matjson::Value attributesVal = newChildVal["attributes"];
                CCSprite* spr;
                std::string sprType = "sprite";
                if (attributesVal.contains("sprite-frame")) sprType = "sprite-frame";
                if ((attributesVal.contains(sprType) && attributesVal[sprType].isString())) {
                    std::string spriteName = attributesVal[sprType].asString().unwrapOr("");
                    spr = Utils::getValidSpriteFrame(spriteName.c_str());
                    if (!spr) spr = Utils::getValidSprite(spriteName.c_str());
                    if (!spr) spr = CCSprite::create();
                }
                newNode = CCMenuItemSpriteExtra::create(spr, nullptr, nullptr, nullptr);
            } else if (type == "CCScale9Sprite") {
                if (newChildVal.contains("attributes")) {
                    matjson::Value attributesVal = newChildVal["attributes"];
                    if (attributesVal.contains("sprite") && attributesVal["sprite"].isString()) {
                        std::string sprite = attributesVal["sprite"].asString().unwrapOr("");
                        newNode = CCScale9Sprite::create(sprite.c_str());
                    }
                }
            } else if (type == "Alert") {
                if (newChildVal.contains("attributes")) {
                    matjson::Value attributesVal = newChildVal["attributes"];
                    if (attributesVal.contains("title") && attributesVal.contains("description")) {
                        std::string title = attributesVal["title"].asString().unwrapOr("");
                        std::string description = attributesVal["description"].asString().unwrapOr("");
                        std::string buttonText = "Okay";
                        if (attributesVal.contains("button-text")) {
                            buttonText = attributesVal["button-text"].asString().unwrapOr("");
                        }
                        FLAlertLayer* alert = geode::createQuickPopup(title.c_str(), description, buttonText.c_str(), nullptr, nullptr, false, true);
                        reinterpret_cast<MyFLAlertLayer*>(alert)->setRift();
                        DataNode* data = DataNode::create(alert);
                        newNode = data;
                    }
                }
            } else if (type == "Popup") {
                std::string sprite = "GJ_square01.png";
                std::string title = "";
                float width = 60, height = 60;

                if (newChildVal.contains("attributes")) {
                    matjson::Value attributesVal = newChildVal["attributes"];
                    if (attributesVal.contains("sprite") && attributesVal["sprite"].isString()) {
                        sprite = attributesVal["sprite"].asString().unwrapOr("");
                    }
                    if (attributesVal.contains("popup-size") && attributesVal["popup-size"].isObject()) {
                        matjson::Value size = attributesVal["popup-size"];
                        width = size["width"].asDouble().unwrapOr(60);
                        height = size["height"].asDouble().unwrapOr(60);
                    }
                    if (attributesVal.contains("title") && attributesVal["title"].isString()) {
                        title = attributesVal["title"].asString().unwrapOr("");
                    }
                }

                CustomAlert* alert = CustomAlert::create(width, height, sprite, title);
                DataNode* data = DataNode::create(alert);
                newNode = data;
            }

            if (newNode) {
                MyCCNode* myNode = static_cast<MyCCNode*>(newNode);
                matjson::Value idVal = newChildVal["id"];
                std::string fullID;
                std::string id;
                if (idVal.isString()) {
                    id = idVal.asString().unwrapOr("");
                    std::string packName = newChildVal.contains("_pack-name") && newChildVal["_pack-name"].isString() 
                                           ? newChildVal["_pack-name"].asString().unwrapOr("") 
                                           : "missing";
                    fullID = fmt::format("{}/{}", packName, id);

                    newNode->setID(fullID.c_str());
                    myNode->setBaseID(id);
                }

                if (DataNode* data = typeinfo_cast<DataNode*>(newNode)) {
                    if (FLAlertLayer* alert = typeinfo_cast<FLAlertLayer*>(data->m_data)) {
                        MyCCNode* myAlert = reinterpret_cast<MyCCNode*>(data);
                        alert->setID(fullID.c_str());
                        myAlert->setBaseID(id);
                    }
                    MyCCNode* myData = reinterpret_cast<MyCCNode*>(data);
                    data->setID(fullID.c_str());
                    myData->setBaseID(id);
                }

                if (FLAlertLayer* alert = typeinfo_cast<FLAlertLayer*>(node)) {
                    alert->m_mainLayer->removeChildByID(fullID);
                    alert->m_mainLayer->addChild(newNode);
                } else {
                    node->removeChildByID(fullID);
                    node->addChild(newNode);
                }
                handleModifications(newNode, newChildVal);
            }
        }
    }
}

void UIModding::handleModifications(CCNode* node, matjson::Value nodeObject, bool transition) {

    if (DataNode* data = typeinfo_cast<DataNode*>(node)){
        node = data->m_data;
    }

    if (!node) return;

    if (transition) {
        if (nodeObject.contains("after-transition") && nodeObject["after-transition"].isObject()) {
            nodeObject = nodeObject["after-transition"];
        }
        else return;
    }
    else {
        static_cast<MyCCNode*>(node)->setAttributes(nodeObject);
    }

    std::string packName = nodeObject.contains("_pack-name") ? nodeObject["_pack-name"].asString().unwrapOr("missing") : "missing";

    if (nodeObject.contains("if") && nodeObject["if"].isArray()) {
        evaluateIf(node, nodeObject["if"].asArray().unwrap());
    }

    if (nodeObject.contains("attributes") && nodeObject["attributes"].isObject()) {
        matjson::Value nodeAttributes = nodeObject["attributes"];
        nodeAttributes["_pack-name"] = packName;
        handleAttributes(node, nodeAttributes);
    }

    if (nodeObject.contains("schedule") && nodeObject["schedule"].isObject()) {
        matjson::Value scheduleVal = nodeObject["schedule"];
        scheduleVal["_pack-name"] = packName;
        handleSchedule(node, scheduleVal);
    }

    if (nodeObject.contains("event") && nodeObject["event"].isObject()) {
        matjson::Value eventVal = nodeObject["event"];
        eventVal["_pack-name"] = packName;
        handleEvent(node, eventVal);
    }

    if (nodeObject.contains("parent") && nodeObject["parent"].isObject()) {
        matjson::Value parentVal = nodeObject["parent"];
        parentVal["_pack-name"] = packName;
        handleParent(node, parentVal);
    }

    if (nodeObject.contains("root") && nodeObject["root"].isObject()) {
        matjson::Value rootVal = nodeObject["root"];
        rootVal["_pack-name"] = packName;
        handleRoot(node, rootVal);
    }

    if (nodeObject.contains("children") && nodeObject["children"].isObject()) {
        matjson::Value childrenVal = nodeObject["children"];
        childrenVal["_pack-name"] = packName;
        handleChildren(node, childrenVal);
    }
}

void UIModding::reloadChildren(CCNode* parentNode, bool transition) {
    if (!parentNode) return;
    MyCCNode* myParentNode = static_cast<MyCCNode*>(parentNode);
    if (!checkNodeValidity(myParentNode)) return;
    
    for (const matjson::Value& value : myParentNode->getAttributes()) {
        handleModifications(myParentNode, value, transition);
    }
    for (auto node : CCArrayExt<MyCCNode*>(myParentNode->getChildren())) {
        reloadChildren(node, transition);
    }
}

void UIModding::refreshChildren(CCNode* parentNode) {
    if (!parentNode) return;
    MyCCNode* myParentNode = static_cast<MyCCNode*>(parentNode);
    doUICheckForType(AlphaUtils::Cocos::getClassName(parentNode, true), parentNode);
    for (auto node : CCArrayExt<MyCCNode*>(myParentNode->getChildren())) {
        refreshChildren(node);
    }
}

void UIModding::cleanChildren(CCNode* parentNode) {
    if (!parentNode) return;
    MyCCNode* myParentNode = static_cast<MyCCNode*>(parentNode);
    myParentNode->clearAttributes();
    myParentNode->resetScheduledAttributes();
    for (auto* node : CCArrayExt<MyCCNode*>(myParentNode->getChildren())) {
        cleanChildren(node);
    }
}

void UIModding::startFileListeners() {
    for (auto& fw : listeners) {
        fw->stop();
    }
    listeners.clear();

    const auto& packs = Utils::getActivePacks();
    for (const auto& pack : packs) {
        std::filesystem::path uiPath = pack.resourcesPath / "ui";

        auto fw = new FileWatcher(uiPath, std::chrono::milliseconds(500));

        listeners.push_back(fw);

        std::thread([this, fw, pack]() mutable {
            fw->start([this, pack](std::filesystem::path pathToWatch, FileStatus status) -> void {
                if (!std::filesystem::is_regular_file(pathToWatch) && status != FileStatus::erased) {
                    return;
                }

                Loader::get()->queueInMainThread([this, pack]{
                    CCScene* scene = CCDirector::get()->getRunningScene();

                    cleanChildren(scene);
                    HPTCCNode::resetAllFromPack(pack.id);

                    Utils::clearCaches();
                    Utils::reloadFileNames();
                    UIModding::get()->loadNodeFiles();
                    Config::get()->loadPackJsons();

                    refreshChildren(scene);

                });
            });
        }).detach();
    }
}

AxisAlignment UIModding::getAxisAlignment(const std::string& name) {
    if (name == "start") {
        return AxisAlignment::Start;
    } else if (name == "center") {
        return AxisAlignment::Center;
    } else if (name == "end") {
        return AxisAlignment::End;
    } else if (name == "even") {
        return AxisAlignment::Even;
    } else if (name == "between") {
        return AxisAlignment::Between;
    }
    return AxisAlignment::Start;
}

MainAxisAlignment UIModding::getSimpleMainAxisAlignment(const std::string& name) {
    if (name == "start") {
        return MainAxisAlignment::Start;
    } else if (name == "center") {
        return MainAxisAlignment::Center;
    } else if (name == "end") {
        return MainAxisAlignment::End;
    } else if (name == "even") {
        return MainAxisAlignment::Even;
    } else if (name == "between") {
        return MainAxisAlignment::Between;
    } else if (name == "around") {
        return MainAxisAlignment::Around;
    }
    return MainAxisAlignment::Start;
}

CrossAxisAlignment UIModding::getSimpleCrossAxisAlignment(const std::string& name) {
    if (name == "start") {
        return CrossAxisAlignment::Start;
    } else if (name == "center") {
        return CrossAxisAlignment::Center;
    } else if (name == "end") {
        return CrossAxisAlignment::End;
    }
    return CrossAxisAlignment::Start;
}

void UIModding::loadNodeFiles() {
    const auto& packs = Utils::getActivePacks();
    
    for (const auto& pack : packs) {
        auto path = pack.resourcesPath;
        std::filesystem::path nodePath = path / "ui";
        
        if (std::filesystem::is_directory(nodePath)) {
            for (const auto& entry : std::filesystem::directory_iterator(nodePath)) {
                const std::string fileName = utils::string::pathToString(entry.path().filename());
                std::vector<std::string> parts = utils::string::split(fileName, ".");
                std::string type = parts.empty() ? "" : parts[0];

                auto fileData = utils::file::readString(entry.path());

                auto extension = utils::string::toLower(utils::string::pathToString(entry.path().extension()));

                if (fileData.isOk()) {
                    if (extension == ".json") {
                        geode::Result<matjson::Value, matjson::ParseError> result = matjson::parse(fileData.unwrap());
                        if (result.isOk()) {
                            matjson::Value obj = result.unwrap();
                            std::string id = pack.id;
                            if (id.empty()) id = pack.name;
                            obj["_pack-name"] = id;
                            obj["after-transition"]["_pack-name"] = id;
                            uiCache[type].push_back(obj);
                        }
                    }
                    else if (extension == ".hpt") {
                        std::string id = pack.id;
                        if (id.empty()) id = pack.name;
                        uiCacheHpt[type].push_back({id, fileData.unwrap()});
                    }
                }
            }
        }
    }
}

void UIModding::doUICheckForType(std::string_view type, CCNode* node) {
    if (skipCheck) return;

    auto it = uiCache.find(type);
    if (it != uiCache.end()) {
        const auto& vec = it->second;
        bool firstMenuLayer = false;
        if (UIModding::get()->firstMenuLayer && type == "MenuLayer") {
            firstMenuLayer = true;
            UIModding::get()->firstMenuLayer = false;
        }
        
        for (auto it = vec.rbegin(); it != vec.rend(); ++it) {
            handleModifications(node, *it);
            if (firstMenuLayer) {
                handleModifications(node, *it, true);
            }
            for (const auto& [k, v] : moveQueue) {
                v();
            }
            moveQueue.clear();
        }
    }

    auto itHpt = uiCacheHpt.find(type);
    if (itHpt != uiCacheHpt.end()) {
        const auto& hptVec = itHpt->second;
        for (auto it = hptVec.rbegin(); it != hptVec.rend(); ++it) {
            const auto& nodes = HPTParser::get().parse(it->data, it->packName, type, node);

            if (firstMenuLayer) {
                //handleModifications(node, *it, true);
            }
        }
    }

    for (auto node : removalQueue) {
        node->removeFromParent();
    }
    removalQueue.clear();
}

bool UIModding::checkNodeValidity(CCObject* node) {
    auto ret = *(uintptr_t*)node == m_nodeVTables[node];
    m_nodeVTables.erase(node);
    return ret;
}

void UIModding::setNodeVTable(CCNode* node) {
    m_nodeVTables[node] = *(uintptr_t*)node;
}

bool UIModding::isNode(CCObject* obj) {
    const std::type_info* ti = &typeid(*obj);

    auto it = m_isNodeCache.find(ti);
    if (it != m_isNodeCache.end()) {
        return it->second;
    }

    bool isNode = typeinfo_cast<CCNode*>(obj) && !typeinfo_cast<GJGameLevel*>(obj);
    m_isNodeCache.emplace(ti, isNode);

    return isNode;
}

UIModding* UIModding::get() {

    if (!instance) {
        instance = new UIModding();
    };
    return instance;
}