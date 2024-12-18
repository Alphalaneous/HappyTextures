#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include "Macros.h"
#include "FileWatcher.h"
#include "Utils.h"
#include "nodes/CCLabelBMFont.h"
#include "nodes/CCMenuItemSpriteExtra.h"
#include "nodes/FLAlertLayer.h"
#include "UIModding.h"
#include "DataNode.h"
#include "alerts/CustomAlert.h"
#include "Callbacks.h"

using namespace geode::prelude;

UIModding* UIModding::instance = nullptr;

$execute {
    UIModding::get()->doModify = Mod::get()->getSettingValue<bool>("ui-modifications");
}

void UIModding::updateColors(CCNode* node, std::string name) {
    if (CCScale9Sprite* bg = typeinfo_cast<CCScale9Sprite*>(node->getChildByIDRecursive(name))) {
        std::optional<ColorData> dataOpt = getColors(name);
        if (dataOpt.has_value()) {
            ColorData data = dataOpt.value();
            bg->setColor(data.color);
            bg->setOpacity(data.alpha);
        }
    }
}

std::optional<ColorData> UIModding::getColors(std::string name) {

    if (colorCache.contains(name)) {
        if (colorCache[name].hasColor) {
            return colorCache[name];
        }
        else {
            return std::nullopt;
        }
    }

    std::string path = "ui/colors.json";

    unsigned long fileSize = 0;
    unsigned char* buffer = CCFileUtils::sharedFileUtils()->getFileData(path.c_str(), "rb", &fileSize);    

    if (buffer && fileSize != 0) {
        
        std::string data = std::string(reinterpret_cast<char*>(buffer), fileSize);
        geode::Result<matjson::Value, matjson::ParseError> value = matjson::parse(data);

        if (value.isOk()) {
            matjson::Value expandedValue = value.unwrap();
            if (expandedValue.isObject() && expandedValue.contains(name)) {
                matjson::Value colorsVal = expandedValue[name];
                if (colorsVal.isObject() && colorsVal.contains("r") && colorsVal.contains("g") && colorsVal.contains("b")) {
                    matjson::Value rValue = colorsVal["r"];
                    matjson::Value gValue = colorsVal["g"];
                    matjson::Value bValue = colorsVal["b"];
                    unsigned char alpha = 255;
                    if (colorsVal.contains("a")) {
                        matjson::Value aValue = colorsVal["a"];
                        if (aValue.isNumber()) {
                            alpha = aValue.asInt().unwrapOr(255);
                        }
                    }

                    if (rValue.isNumber() && gValue.isNumber() && bValue.isNumber()) {
                        unsigned char r = rValue.asInt().unwrapOr(0);
                        unsigned char g = gValue.asInt().unwrapOr(0);
                        unsigned char b = bValue.asInt().unwrapOr(0);

                        ColorData data = ColorData{ccColor3B{r, g, b}, alpha, true};
                        colorCache.insert({name, data});
                        delete[] buffer;
                        return data;
                    }
                }
            }
        }
    }
    ColorData data = ColorData{ccColor3B{0, 0, 0}, 0, false};
    colorCache.insert({name, data});
    delete[] buffer;

    return std::nullopt;
}

void UIModding::recursiveModify(CCNode* node, matjson::Value elements) {

    CCArray* children = node->getChildren();
    if (CCArray* pageChildren = typeinfo_cast<CCArray*>(node->getUserObject("alphalaneous.pages_api/page-children"))){
        children = pageChildren;
    }

    for (CCNode* node : CCArrayExt<CCNode*>(children)) {

        std::string id = node->getID();

        if (elements.contains("_pack-name") && elements["_pack-name"].isString()) {
            std::string identifier = fmt::format("{}/", elements["_pack-name"].asString().unwrapOr(""));
            id = Utils::strReplace(node->getID(), identifier, "");
        }
        
        if (elements.contains(id)) {
            matjson::Value nodeValue = elements[id];
            if (nodeValue.isObject()) {
                handleModifications(node, nodeValue);
            }
        }
    }
}

void UIModding::runAction(CCNode* node, matjson::Value attributes) {

    #ifndef GEODE_IS_MACOS

    if (attributes.contains("actions")) {
        matjson::Value actionsValue = attributes["actions"];
        if (actionsValue.isArray()) {
            
            CCArray* actionArray = CCArray::create();
            geode::Result<std::vector<matjson::Value>&> actionValues = actionsValue.asArray();

            if (actionValues.isOk()) {
                for (matjson::Value action : actionValues.unwrap()) {
                    if (action.isObject()) {
                        CCActionInterval* actionInterval = createAction(node, action);

                        if (actionInterval) {
                            actionArray->addObject(actionInterval);
                        }
                    }
                }
                if (actionArray->count() > 0) {
                    node->runAction(CCSpawn::create(actionArray));
                }
            }
        }
    }
    #endif
}
std::string formatAddressIntoOffsetImpl(uintptr_t addr, bool module) {
    HMODULE mod;

    if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<char*>(addr), &mod)
    ) {
        mod = nullptr;
    }

    wchar_t buffer[MAX_PATH];
    std::string const module_name = (!mod || !GetModuleFileNameW(mod, buffer, MAX_PATH)) ? "Unknown" : std::filesystem::path(buffer).filename().string();

    if(module) return fmt::format("{} + {:#x}", module_name, addr - reinterpret_cast<uintptr_t>(mod));
    return fmt::format("{:#x}", addr - reinterpret_cast<uintptr_t>(mod));
}

std::string formatAddressIntoOffset(uintptr_t addr, bool module) {
    static std::unordered_map<uintptr_t, std::pair<std::string, std::string>> formatted;
    auto it = formatted.find(addr);
    if (it != formatted.end()) {
        if(module) return it->second.first;
        else return it->second.second;
    } else {
        auto const txt = formatAddressIntoOffsetImpl(addr, true);
        auto const txtNoModule = formatAddressIntoOffsetImpl(addr, false);
        auto const pair = std::make_pair(txt, txtNoModule);
        formatted.insert({ addr, pair });
        if(module) return pair.first;
        else return pair.second;
    }
}

void UIModding::runCallback(CCNode* node, matjson::Value attributes) {
    if (attributes.contains("callback")) {
        matjson::Value callbackValue = attributes["callback"];

        if (callbackValue.contains("class") && callbackValue.contains("method")) {
            std::string className = callbackValue["class"].asString().unwrapOr("");
            std::string methodName = callbackValue["method"].asString().unwrapOr("");

            if (Callbacks::get()->m_callbacks.contains(className)) {
                std::map<std::string, std::pair<CCNode*, cocos2d::SEL_MenuHandler>> callbackMap = Callbacks::get()->m_callbacks[className];
                if (callbackMap.contains(methodName)) {
                    std::pair<CCNode*, cocos2d::SEL_MenuHandler> callback = callbackMap[methodName];
		            (callback.first->*callback.second)(Callbacks::get()->getDummyButton());
                }
            }
        }
    }
}

CCActionInterval* UIModding::createAction(CCNode* node, matjson::Value action) {
    CCActionInterval* retAction = nullptr;

    #ifndef GEODE_IS_MACOS

    if (action.contains("type")) {

        bool repeat = false;

        float value = 0;
        float duration = 0;
        float x = 0;
        float y = 0;
        float easingRate = 0;
        int repeatCount = INT32_MAX;

        std::string easingType = "none";
        std::string link = "";

        CCActionInterval* actionToDo = nullptr;

        if (action.contains("easing")) {
            matjson::Value easingValue = action["easing"];
            if (easingValue.isString()) {
                easingType = easingValue.asString().unwrapOr("none");
            }
        }
        if (action.contains("easing-rate")) {
            matjson::Value easingRateVal = action["easing-rate"];
            if (easingRateVal.isNumber()) {
                easingRate = easingRateVal.asDouble().unwrapOr(0);
            }
        }
        if (action.contains("repeat")) {
            matjson::Value repeatVal = action["repeat"];
            if (repeatVal.isBool()) {
                repeat = repeatVal.asBool().unwrapOr(false);
            }
            if (repeatVal.isNumber()) {
                repeatCount = repeatVal.asInt().unwrapOr(INT32_MAX);
            }
        }
        if (action.contains("duration")) {
            matjson::Value durationValue = action["duration"];
            if (durationValue.isNumber()) {
                duration = durationValue.asDouble().unwrapOr(0);
            }
        }

        bool isNumber = false;

        if (action.contains("value")) {
            matjson::Value valueValue = action["value"];
            if (valueValue.isNumber()) {
                isNumber = true;
                value = valueValue.asDouble().unwrapOr(0);
            }
            else if (valueValue.isObject()) {
                if (valueValue.contains("x") && valueValue.contains("y")) {
                    matjson::Value xValue = valueValue["x"];
                    matjson::Value yValue = valueValue["y"];

                    if (xValue.isNumber() && yValue.isNumber()) {
                        x = xValue.asDouble().unwrapOr(0);
                        y = yValue.asDouble().unwrapOr(0);
                    }
                }
                if (valueValue.contains("width") && valueValue.contains("height")) {
                    matjson::Value wValue = valueValue["width"];
                    matjson::Value hValue = valueValue["height"];

                    if (wValue.isNumber() && hValue.isNumber()) {
                        x = wValue.asDouble().unwrapOr(0);
                        y = hValue.asDouble().unwrapOr(0);
                    }
                }
            }
        }

        matjson::Value typeValue = action["type"];
        if (typeValue.isString()) {
            std::string type = typeValue.asString().unwrapOr("");

            if (type == "Sequence") {

                CCArray* sequentialActions = CCArray::create();

                if (action.contains("actions")) {

                    matjson::Value seqActions = action["actions"];
                    if (seqActions.isArray()) {
                        geode::Result<std::vector<matjson::Value>&> seqActionsArr = seqActions.asArray();

                        if (seqActionsArr.isOk()) {
                            for (matjson::Value seqAction : seqActionsArr.unwrap()) {

                                CCActionInterval* seqActionToDo = createAction(node, seqAction);
                                if (seqActionToDo) {
                                    sequentialActions->addObject(seqActionToDo);
                                }
                            }
                        }
                    }
                }

                if (sequentialActions->count() > 0) {
                    actionToDo = CCSequence::create(sequentialActions);
                }
            }

            if (type == "Stop") { 
                node->stopAllActions();
            }

            actionForName(MoveBy, (duration, {x, y}));
            actionForName(MoveTo, (duration, {x, y}));
            actionForName(SkewBy, (duration, x, y));
            actionForName(SkewTo, (duration, x, y));
            actionForName(FadeIn, (duration));
            actionForName(FadeOut, (duration));
            actionForName(FadeTo, (duration, value));
            actionForName2(ScaleTo, x, y, value);
            actionForName2(ScaleBy, x, y, value);
            actionForName2(RotateBy, x, y, value);
            actionForName2(RotateTo, x, y, value);

        }
        if (actionToDo) {

            CCActionInterval* finalAction = getEasingType(easingType, actionToDo, easingRate);
            if (finalAction) {
                if (repeat) {
                    retAction = CCRepeat::create(finalAction, repeatCount);
                }
                else {
                    retAction = finalAction;
                }
            }
        }
    }
    #endif
    return retAction;
}

CCActionInterval* UIModding::getEasingType(std::string name, CCActionInterval* action, float rate) {

    CCActionInterval* easingType = nullptr;
    
    if (name == "none") {
        easingType = action;
    }

#ifndef GEODE_IS_MACOS

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

#endif

    return easingType;
}

void UIModding::setLayout(CCNode* node, matjson::Value attributes) {

    if (attributes.contains("layout")) {

        matjson::Value layoutValue = attributes["layout"];
        if (layoutValue.isObject()) {
            AxisLayout* layout;

            if (node->getLayout()) {
                layout = typeinfo_cast<AxisLayout*>(node->getLayout());
                if (layoutValue.contains("remove")) {
                    matjson::Value removeValue = layoutValue["remove"];
                    if (removeValue.isBool()) {
                        node->setLayout(nullptr);
                    }
                }
            }
            else {
                layout = AxisLayout::create();
                node->setLayout(layout);
            }
            if (layout) {

                if (layoutValue.contains("axis")) {
                    matjson::Value axisValue = layoutValue["axis"];
                    if (axisValue.isString()) {
                        std::string axis = axisValue.asString().unwrapOr("");
                        if (axis == "row") {
                            layout->setAxis(Axis::Row);
                        }
                        else if (axis == "column") {
                            layout->setAxis(Axis::Column);
                        }
                    }
                }
                if (layoutValue.contains("flip-axis")) {
                    matjson::Value flipAxisValue = layoutValue["flip-axis"];
                    if (flipAxisValue.isBool()) {
                        bool flipAxis = flipAxisValue.asBool().unwrapOr(false);
                        layout->setAxisReverse(flipAxis);
                    }
                }
                if (layoutValue.contains("ignore-invisible")) {
                    matjson::Value ignoreInvisibleValue = layoutValue["ignore-invisible"];
                    if (ignoreInvisibleValue.isBool()) {
                        bool ignoreInvisible = ignoreInvisibleValue.asBool().unwrapOr(false);
                        layout->ignoreInvisibleChildren(ignoreInvisible);
                    }
                }
                if (layoutValue.contains("flip-cross-axis")) {
                    matjson::Value flipCrossAxisValue = layoutValue["flip-cross-axis"];
                    if (flipCrossAxisValue.isBool()) {
                        bool flipCrossAxis = flipCrossAxisValue.asBool().unwrapOr(false);
                        layout->setCrossAxisReverse(flipCrossAxis);
                    }
                }
                if (layoutValue.contains("auto-scale")) {
                    matjson::Value autoScaleValue = layoutValue["auto-scale"];
                    if (autoScaleValue.isBool()) {
                        bool autoScale = autoScaleValue.asBool().unwrapOr(false);
                        layout->setAutoScale(autoScale);
                    }
                }
                if (layoutValue.contains("grow-cross-axis")) {
                    matjson::Value growCrossAxisValue = layoutValue["grow-cross-axis"];
                    if (growCrossAxisValue.isBool()) {
                        bool growCrossAxis = growCrossAxisValue.asBool().unwrapOr(false);
                        layout->setGrowCrossAxis(growCrossAxis);
                    }
                }
                if (layoutValue.contains("allow-cross-axis-overflow")) {
                    matjson::Value allowCrossAxisOverflowValue = layoutValue["allow-cross-axis-overflow"];
                    if (allowCrossAxisOverflowValue.isBool()) {
                        bool allowCrossAxisOverflow = allowCrossAxisOverflowValue.asBool().unwrapOr(false);
                        layout->setCrossAxisOverflow(allowCrossAxisOverflow);
                    }
                }
                if (layoutValue.contains("gap")) {
                    matjson::Value gapValue = layoutValue["gap"];
                    if (gapValue.isNumber()) {
                        float gap = gapValue.asDouble().unwrapOr(0);
                        layout->setGap(gap);
                    }
                }
                if (layoutValue.contains("axis-alignment")) {
                    matjson::Value axisAlignmentValue = layoutValue["axis-alignment"];
                    if (axisAlignmentValue.isString()) {
                        std::string axisAlignmentStr = axisAlignmentValue.asString().unwrapOr("");
                        AxisAlignment axisAlignment = getAxisAlignment(axisAlignmentStr);
                        layout->setAxisAlignment(axisAlignment);
                    }
                }
                if (layoutValue.contains("cross-axis-alignment")) {
                    matjson::Value crossAxisAlignmentValue = layoutValue["cross-axis-alignment"];
                    if (crossAxisAlignmentValue.isString()) {
                        std::string crossAxisAlignmentStr = crossAxisAlignmentValue.asString().unwrapOr("");
                        AxisAlignment axisAlignment = getAxisAlignment(crossAxisAlignmentStr);
                        layout->setCrossAxisAlignment(axisAlignment);
                    }
                }
                if (layoutValue.contains("cross-axis-line-alignment")) {
                    matjson::Value crossAxisLineAlignmentValue = layoutValue["cross-axis-line-alignment"];
                    if (crossAxisLineAlignmentValue.isString()) {
                        std::string crossAxisLineAlignmentStr = crossAxisLineAlignmentValue.asString().unwrapOr("");
                        AxisAlignment axisAlignment = getAxisAlignment(crossAxisLineAlignmentStr);
                        layout->setCrossAxisLineAlignment(axisAlignment);
                    }
                }
            }
            node->updateLayout();
        }
    }
}

std::string UIModding::getSound(std::string sound) {

    std::string soundRet = "";

    gd::vector<gd::string> paths = CCFileUtils::sharedFileUtils()->getSearchPaths();

    for (std::string path : paths) {
        std::string soundPathStr = fmt::format("{}{}", path, sound);
        std::filesystem::path soundPath = std::filesystem::path(soundPathStr);
        if (std::filesystem::exists(soundPath)) {
            soundRet = soundPath.string();
            break;
        }
    }

    return soundRet;
}

void UIModding::playSound(CCNode* node, matjson::Value attributes) {

    if (attributes.contains("sound")) {
        matjson::Value soundVal = attributes["sound"];
        if (soundVal.isString()) {
            std::string sound = soundVal.asString().unwrapOr("");

            FMODAudioEngine::sharedEngine()->m_backgroundMusicChannel->setPaused(false);
            FMODAudioEngine::sharedEngine()->m_globalChannel->setPaused(false);
            FMODAudioEngine::sharedEngine()->m_channelGroup2->setPaused(false);
            FMODAudioEngine::sharedEngine()->m_system->update();
            std::string soundPath = getSound(sound);
            if (!soundPath.empty()) {
                FMODAudioEngine::sharedEngine()->playEffectAdvanced(soundPath, 1, 0, 1, 1, true, false, 0, 0, 0, 0, false, 0, false, true, 0, 0, 0, 0);
            }
        }
    }
}

void UIModding::openLink(CCNode* node, matjson::Value attributes) {
    if (attributes.contains("link")) {
        matjson::Value linkVal = attributes["link"];

        if (linkVal.isString()) {
            std::string link = linkVal.asString().unwrapOr("");
            web::openLinkInBrowser(link);
        }
        if (linkVal.isObject()) {
            if (linkVal.contains("type") && linkVal.contains("id")) {
                matjson::Value typeVal = linkVal["type"];
                matjson::Value idVal = linkVal["id"];

                if (typeVal.isString() && idVal.isNumber()) {
                    std::string type = typeVal.asString().unwrapOr("");
                    int id = idVal.asInt().unwrapOr(0);

                    if (type == "profile") {
                        ProfilePage::create(id, false)->show();
                    }
                    if (type == "level") {
                        auto searchObject = GJSearchObject::create(SearchType::Type19, fmt::format("{}&gameVersion=22", id));
                        auto scene = LevelBrowserLayer::scene(searchObject);
                        CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, scene));
                    }
                }
            }
        }
    }
}

void UIModding::setShow(CCNode* node, matjson::Value attributes) {
    if (attributes.contains("show")) {
        matjson::Value showVal = attributes["show"];
        if (showVal.isBool()) {
            bool show = showVal.asBool().unwrapOr(false);
            if (FLAlertLayer* alert = typeinfo_cast<FLAlertLayer*>(node)) {
                if (show) alert->show();
                else alert->keyBackClicked();  
            }
        }
    }
}

void UIModding::setZOrder(CCNode* node, matjson::Value attributes) {
    if (attributes.contains("z-order")) {
        matjson::Value zOrderVal = attributes["z-order"];
        if (zOrderVal.isNumber()) {
            int zOrder = zOrderVal.asInt().unwrapOr(0);
            node->setZOrder(zOrder);
        }
    }
}

void UIModding::setBlending(CCNode* node, matjson::Value attributes) {

    if (attributes.contains("blending")) {
        matjson::Value blendingVal = attributes["blending"];
        if (blendingVal.isObject()) {
            if (blendingVal.contains("source") && blendingVal.contains("destination")) {
                matjson::Value sourceVal = blendingVal["source"];
                matjson::Value destinationVal = blendingVal["destination"];

                if (sourceVal.isString() && destinationVal.isString()) {

                    std::string source = sourceVal.asString().unwrapOr("");
                    std::string destination = destinationVal.asString().unwrapOr("");

                    unsigned int src = stringToBlendingMode(source);
                    unsigned int dst = stringToBlendingMode(destination);

                    if (src != -1 && dst != -1) {
                        if (CCBlendProtocol* blendable = typeinfo_cast<CCBlendProtocol*>(node)) {
                            blendable->setBlendFunc({src, dst});
                        }
                    }
                }
            }
        }
    }
}

std::map<std::string, unsigned int> strBlend = {
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

unsigned int UIModding::stringToBlendingMode(std::string value) {
    unsigned int val = -1;
    if (strBlend.contains(value)) val = strBlend[value];
    return val;
}

void UIModding::setFlip(CCNode* node, matjson::Value attributes) {
    if (attributes.contains("flip")) {
        matjson::Value flipVal = attributes["flip"];
        if (flipVal.isObject()) {
            bool flipX = false;
            bool flipY = false;

            if (flipVal.contains("x")) {
                matjson::Value xVal = flipVal["x"];
                if (xVal.isBool()) flipX = xVal.asBool().unwrapOr(false); 
            }
            if (flipVal.contains("y")) {
                matjson::Value yVal = flipVal["y"];
                if (yVal.isBool()) flipY = yVal.asBool().unwrapOr(false); 
            }
            if (CCSprite* sprite = typeinfo_cast<CCSprite*>(node)) {
                sprite->setFlipX(flipX);
                sprite->setFlipY(flipY);
            }
        }
    }
}


void UIModding::setFont(CCNode* node, matjson::Value attributes) {
    if (attributes.contains("font")) {
        matjson::Value fontVal = attributes["font"];
        if (fontVal.isString()) {
            CCLabelBMFont* textObject;

            std::string font = fontVal.asString().unwrapOr("");
            if (CCLabelBMFont* textNode = typeinfo_cast<CCLabelBMFont*>(node)) {
                textObject = textNode;
            }
            else if (CCMenuItemSpriteExtra* buttonNode = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
                if (SearchButton* searchButton = node->getChildByType<SearchButton>(0)) {
                    textObject = searchButton->getChildByType<CCLabelBMFont>(0);
                }
                else if (ButtonSprite* buttonSprite = node->getChildByType<ButtonSprite>(0)) {
                    textObject = buttonSprite->getChildByType<CCLabelBMFont>(0);
                }
                else {
                    textObject = node->getChildByType<CCLabelBMFont>(0);
                }
            }
            if (textObject) {
                if (utils::string::endsWith(font, ".fnt")) {
                    MyCCLabelBMFont* myTextObject = static_cast<MyCCLabelBMFont*>(textObject);
                    
                    CCLabelBMFont* dummyTextObject = CCLabelBMFont::create("", font.c_str());
                    if (dummyTextObject) {
                        textObject->setFntFile(font.c_str());

                        if (myTextObject->m_fields->m_isLimited) {
                            textObject->limitLabelWidth(myTextObject->m_fields->m_limitWidth, myTextObject->m_fields->m_limitDefaultScale, myTextObject->m_fields->m_limitMinScale);
                        }
                    }
                }
            }
        }
    }
}

void UIModding::setPosition(CCNode* node, matjson::Value attributes) {
    if (attributes.contains("position")) {
        matjson::Value position = attributes["position"];
        if (position.isObject()) {
            
            float x = 0;
            float y = 0;

            if (position.contains("x") && position.contains("y")) {
                matjson::Value xVal = position["x"];
                matjson::Value yVal = position["y"];

                if (xVal.isNumber() && yVal.isNumber()) {
                    x = xVal.asDouble().unwrapOr(0);
                    y = yVal.asDouble().unwrapOr(0);
                }
            }
            if (position.contains("anchor")) {
                matjson::Value anchorValue = position["anchor"];

                if (anchorValue.isObject() || anchorValue.isString()) {
                    CCNode* parent = node->getParent();
                    CCSize nodeSize;
                    if (parent) {
                        nodeSize = parent->getContentSize();
                    }
                    else {
                        nodeSize = CCDirector::sharedDirector()->getWinSize();
                    }

                    if (!anchorValue.isString()) {
                        if (anchorValue.contains("relative")) {
                            matjson::Value relativeValue = anchorValue["relative"];
                            if (relativeValue.isString()) {
                                std::string relative = relativeValue.asString().unwrapOr("");
                                if (relative == "screen") {
                                    nodeSize = CCDirector::sharedDirector()->getWinSize();
                                }
                                else if (relative == "parent") {
                                    nodeSize = node->getParent()->getContentSize();
                                }
                            }
                        }
                    }

                    if (anchorValue.contains("to") || anchorValue.isString()) {
                        matjson::Value anchorToValue;
                        if (anchorValue.isString()) {
                            anchorToValue = anchorValue;
                        }
                        else if (anchorValue.contains("to")) {
                            anchorToValue = anchorValue["to"];
                        }
                        if (anchorToValue.isString()) {
                            std::string anchorTo = anchorToValue.asString().unwrapOr("");
                            if (anchorTo == "top-left") {
                                y += nodeSize.height;
                            }
                            else if (anchorTo == "top-center") {
                                x += nodeSize.width/2;
                                y += nodeSize.height;
                            }
                            else if (anchorTo == "top-right") {
                                x += nodeSize.width;
                                y += nodeSize.height;
                            }
                            else if (anchorTo == "center-left") {
                                y += nodeSize.height/2;
                            }
                            else if (anchorTo == "center") {
                                x += nodeSize.width/2;
                                y += nodeSize.height/2;
                            }
                            else if (anchorTo == "center-right") {
                                x += nodeSize.width;
                                y += nodeSize.height/2;
                            }
                            else if (anchorTo == "bottom-center") {
                                x += nodeSize.width/2;
                            }
                            else if (anchorTo == "bottom-right") {
                                x += nodeSize.width;
                            }
                            else if (anchorTo == "self") {
                                x += node->getPosition().x;
                                y += node->getPosition().y;
                            }
                        }
                    }
                }
            }
            node->setPosition({x, y});
        }
    }
}

void UIModding::setColor(CCNode* node, matjson::Value attributes) {
    if (attributes.contains("color")) {
        matjson::Value color = attributes["color"];
        if (color.isObject()) {
            if (color.contains("r") && color.contains("g") && color.contains("b")) {
                matjson::Value colorR = color["r"];
                matjson::Value colorG = color["g"];
                matjson::Value colorB = color["b"];

                if (colorR.isNumber() && colorG.isNumber() && colorB.isNumber()) {

                    unsigned char r = colorR.asInt().unwrapOr(0);
                    unsigned char g = colorG.asInt().unwrapOr(0);
                    unsigned char b = colorB.asInt().unwrapOr(0);

                    if (CCMenuItemSpriteExtra* node1 = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
                        node1->setColor(ccColor3B{r, g, b});
                        if (ButtonSprite* node2 = node1->getChildByType<ButtonSprite>(0)) {
                            node2->setColor(ccColor3B{r, g, b});
                        }
                    }
                    if (CCRGBAProtocol* node1 = typeinfo_cast<CCRGBAProtocol*>(node)) {
                        node1->setColor(ccColor3B{r, g, b});
                    }
                }
            }
        }
        if (color.isString()) {
            std::string colorStr = color.asString().unwrapOr("");
            if (colorStr == "reset") {
                if (EventCCMenuItemSpriteExtra* node1 = static_cast<EventCCMenuItemSpriteExtra*>(node)) {
                    node1->setColor(node1->m_fields->originalColor);
                    if (ButtonSprite* node2 = node1->getChildByType<ButtonSprite>(0)) {
                        node2->setColor(node1->m_fields->originalColor); 
                    }
                }
            }
        }
    }
}

void UIModding::removeChild(CCNode* node, matjson::Value attributes) {
    if (attributes.contains("remove")) {
        matjson::Value removeVal = attributes["remove"];
        if (removeVal.isBool()) {
            bool remove = removeVal.asBool().unwrapOr(false);
            if (remove) removalQueue->addObject(node);
        }
    }
}

void UIModding::setScaleMult(CCNode* node, matjson::Value attributes) {
    if (attributes.contains("scale-multiplier")) {
        matjson::Value mulVal = attributes["scale-multiplier"];
        if (mulVal.isNumber()) {
            float multiplier = mulVal.asDouble().unwrapOr(0);
            if (CCMenuItemSpriteExtra* button = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
                button->m_scaleMultiplier = multiplier;
            }
        }
    }
}

void UIModding::setScaleBase(CCNode* node, matjson::Value attributes) {
    if (attributes.contains("base-scale")) {
        matjson::Value baseVal = attributes["base-scale"];
        if (baseVal.isNumber()) {
            float base = baseVal.asDouble().unwrapOr(0);
            if (CCMenuItemSpriteExtra* button = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
                button->m_baseScale = base;
            }
        }
    }
}

void UIModding::setDisablePages(CCNode* node, matjson::Value attributes) {
    if (attributes.contains("disable-pages")) {
        matjson::Value pagesVal = attributes["disable-pages"];
        if (pagesVal.isBool()) {
            bool disablePages = pagesVal.asBool().unwrapOr(false);
            if (disablePages) {
                if (CCMenuItemSpriteExtra* disableItem = typeinfo_cast<CCMenuItemSpriteExtra*>(node->getUserObject("alphalaneous.pages_api/disable"))) {
                    (disableItem->m_pListener->*disableItem->m_pfnSelector)(disableItem);
                }
            }
        }
    }
}

void UIModding::setText(CCNode* node, matjson::Value attributes){
    if (attributes.contains("text")) {
        matjson::Value textVal = attributes["text"];
        if (textVal.isString()) {

            CCLabelBMFont* textObject;

            std::string text = textVal.asString().unwrapOr("");
            if (CCLabelBMFont* textNode = typeinfo_cast<CCLabelBMFont*>(node)) {
                textObject = textNode;
            }
            else if (CCMenuItemSpriteExtra* buttonNode = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
                if (SearchButton* searchButton = node->getChildByType<SearchButton>(0)){
                    textObject = searchButton->getChildByType<CCLabelBMFont>(0);
                }
                else if (ButtonSprite* buttonSprite = node->getChildByType<ButtonSprite>(0)) {
                    textObject = buttonSprite->getChildByType<CCLabelBMFont>(0);
                }
                else {
                    textObject = node->getChildByType<CCLabelBMFont>(0);
                }
            }
            if (textObject) {

                MyCCLabelBMFont* myTextObject = static_cast<MyCCLabelBMFont*>(textObject);
                myTextObject->setHappyTexturesModified();
                textObject->setString(text.c_str());

                if(myTextObject->m_fields->m_isLimited){
                    textObject->limitLabelWidth(myTextObject->m_fields->m_limitWidth, myTextObject->m_fields->m_limitDefaultScale, myTextObject->m_fields->m_limitMinScale);
                }
            }
        }
    }
}

std::vector<std::string> generateValidSprites(std::string path, matjson::Value spriteList) {

    std::vector<std::string> sprites;
    std::vector<std::string> validSprites;

    if (!path.empty()) {
        std::vector<std::string> packs = Utils::getActivePacks();
        for (std::string packPath : packs) {
            std::string sprPath = fmt::format("{}{}", packPath, path);
            if (std::filesystem::is_directory(sprPath)) {
                for (const auto& entry : std::filesystem::directory_iterator(sprPath)) {
                    std::string textureName = utils::string::split(entry.path().filename().string(), ".").at(0);
                    if (!utils::string::endsWith(textureName, "-hd") && !utils::string::endsWith(textureName, "-uhd")) {
                        std::string sprName = fmt::format("{}\\{}", path, entry.path().filename().string());
                        sprites.push_back(sprName);
                    }
                }
            }
        }
    }

    if (spriteList.isArray()) {
        for (matjson::Value v : spriteList.asArray().unwrap()) {
            if (v.isString()) sprites.push_back(v.asString().unwrapOr(""));
        }
    }

    for (std::string sprStr : sprites) {
        if (Utils::getValidSprite(sprStr.c_str())) {
            validSprites.push_back(sprStr);
        }
    }
    return validSprites;
}

void UIModding::setSprite(CCNode* node, matjson::Value attributes) {
    if (attributes.contains("sprite")) {
        matjson::Value sprite = attributes["sprite"];
        CCSprite* spr;
        std::string spriteName;
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
            spr = Utils::getValidSprite(spriteName.c_str());
            if (!spr) return;
        }
        if (sprite.isString()) {
            spriteName = sprite.asString().unwrapOr("");
            spr = Utils::getValidSprite(spriteName.c_str());
            if (!spr) return;
        }

        if (CCSprite* spriteNode = typeinfo_cast<CCSprite*>(node)) {
            spriteNode->setTexture(spr->getTexture());
            spriteNode->setTextureRect(spr->getTextureRect());
        }
        if (CCMenuItemSpriteExtra* buttonNode = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {

            if (attributes.contains("button-info")) {
                matjson::Value infoVal = attributes["button-info"];
                if (infoVal.isObject()) {
                    if (infoVal.contains("type")) {
                        matjson::Value typeVal = infoVal["type"];
                        if (typeVal.isString()) {
                            std::string type = typeVal.asString().unwrapOr("");
                            if (type == "labeled") {

                                std::string caption = "";
                                int width = 30;
                                bool absolute = false;
                                std::string font = "bigFont.fnt";
                                std::string texture = "GJ_button_01.png";
                                float height = 30;
                                float scale = 1;
                                
                                setSpriteVar(caption, text, String, "");
                                setSpriteVar(font, font, String, "");
                                setSpriteVar(texture, sprite, String, "");
                                setSpriteVarNum(width, width, Int, 0);
                                setSpriteVarNum(height, height, Double, 0);
                                setSpriteVarNum(scale, scale, Double, 0);
                                setSpriteVar(absolute, absolute, Bool, false);
                                
                                CCSprite* spr = Utils::getValidSprite(texture.c_str());
                                if (!spr) return;

                                CCLabelBMFont* fnt = CCLabelBMFont::create("", font.c_str());

                                if (spr && fnt) {
                                    auto buttonSprite = ButtonSprite::create(caption.c_str(), width, absolute, font.c_str(), texture.c_str(), height, scale);
                                    buttonNode->setNormalImage(buttonSprite);
                                    Utils::updateSprite(buttonNode);
                                }

                            }
                            else if (type == "sprite") {
                                buttonNode->setNormalImage(spr);
                                Utils::updateSprite(buttonNode);
                            }
                        }
                    }
                }
            }
            if (ButtonSprite* buttonSprite = node->getChildByType<ButtonSprite>(0)) {
                buttonSprite->updateBGImage(spriteName.c_str());
            }
            else if (CCSprite* sprite = node->getChildByType<CCSprite>(0)) {
                sprite->setTexture(spr->getTexture());
                sprite->setTextureRect(spr->getTextureRect());
                sprite->setContentSize(spr->getContentSize());
                buttonNode->setContentSize(spr->getContentSize());
                sprite->setPosition(buttonNode->getContentSize()/2);
            }
        }
        
    }
}

void UIModding::setSpriteFrame(CCNode* node, matjson::Value attributes) {
    if (attributes.contains("sprite-frame")) {
        matjson::Value sprite = attributes["sprite-frame"];
        if (sprite.isString()) {
            std::string spriteName = sprite.asString().unwrapOr("");
            if (CCSprite* spriteNode = typeinfo_cast<CCSprite*>(node)) {
                CCSprite* spr = Utils::getValidSpriteFrame(spriteName.c_str());
                if(spr) {
                    spriteNode->setTextureAtlas(spr->getTextureAtlas());
                    spriteNode->setTexture(spr->getTexture());
                    spriteNode->setTextureRect(spr->getTextureRect());
                }
            }
            if (CCMenuItemSpriteExtra* buttonNode = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
                CCSprite* spr = Utils::getValidSpriteFrame(spriteName.c_str());
                if (spr) {
                    if (CCSprite* spriteNode = buttonNode->getChildByType<CCSprite>(0)) {
                        spriteNode->setTexture(spr->getTexture());
                        spriteNode->setTextureRect(spr->getTextureRect());
		                spriteNode->setTextureAtlas(spr->getTextureAtlas());
                        spriteNode->setContentSize(spr->getContentSize());
                        buttonNode->setContentSize(spr->getContentSize());
                        spriteNode->setPosition(buttonNode->getContentSize()/2);
                    }
                }
            }
        }
    }
}

void UIModding::setOpacity(CCNode* node, matjson::Value attributes) {
    if (attributes.contains("opacity")) {
        matjson::Value opacity = attributes["opacity"];
        if (opacity.isNumber()) {
            unsigned char opacityNum = opacity.asInt().unwrapOr(0);
            if (CCMenuItemSpriteExtra* node1 = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
                node1->setOpacity(opacityNum);
                if (ButtonSprite* node2 = node1->getChildByType<ButtonSprite>(0)) {
                    node2->setOpacity(opacityNum);
                }
            }
            if (CCNodeRGBA* nodeRGBA = typeinfo_cast<CCNodeRGBA*>(node)) {
                nodeRGBA->setOpacity(opacityNum);
            }
            if (CCLayerRGBA* nodeRGBA = typeinfo_cast<CCLayerRGBA*>(node)) {
                nodeRGBA->setOpacity(opacityNum);
            }
        }
        if (opacity.isString()) {
            std::string opacityStr = opacity.asString().unwrapOr("");
            if (opacityStr == "reset") {
                if (EventCCMenuItemSpriteExtra* node1 = static_cast<EventCCMenuItemSpriteExtra*>(node)) {
                    node1->setOpacity(node1->m_fields->originalOpacity);
                    if (ButtonSprite* node2 = node1->getChildByType<ButtonSprite>(0)) {
                        node2->setOpacity(node1->m_fields->originalOpacity);
                    }
                }
            }
        }
    }
}

void UIModding::setVisible(CCNode* node, matjson::Value attributes) {

    if (attributes.contains("visible")) {
        matjson::Value visible = attributes["visible"];
        if (visible.isBool()) {
            node->setVisible(visible.asBool().unwrapOr(false));
        }
    }
}

void UIModding::updateLayout(CCNode* node, matjson::Value attributes) {
    if (attributes.contains("update-layout")) {
        matjson::Value update = attributes["update-layout"];
        if (update.isBool()) {
            if (update.asBool().unwrapOr(false)) {
                if (CCNode* parent = node->getParent()) parent->updateLayout();
            }
        }
    }
}

void UIModding::setIgnoreAnchorPos(CCNode* node, matjson::Value attributes) {
    if (attributes.contains("ignore-anchor-pos")) {
        matjson::Value ignore = attributes["ignore-anchor-pos"];
        if (ignore.isBool()) {
            node->ignoreAnchorPointForPosition(ignore.asBool().unwrapOr(false));
        }
    }
}

void UIModding::setScale(CCNode* node, matjson::Value attributes) {
    if (attributes.contains("scale")) {
        matjson::Value scale = attributes["scale"];
        if (scale.isObject()) {
            if (scale.contains("x")) {
                matjson::Value scaleX = scale["x"];
                if (!scaleX.isNull() && scaleX.isNumber()) {
                    node->setScaleX(scaleX.asDouble().unwrapOr(0));
                }
            }
            if (scale.contains("y")) {
                matjson::Value scaleY = scale["y"];
                if (!scaleY.isNull() && scaleY.isNumber()) {
                    node->setScaleY(scaleY.asDouble().unwrapOr(0));
                }
            }
        }

        if (scale.isNumber()) {
            node->setScale(scale.asDouble().unwrapOr(0));
        }
    }
}

void UIModding::setRotation(CCNode* node, matjson::Value attributes) {
    if (attributes.contains("rotation")) {
        matjson::Value rotation = attributes["rotation"];
        if (rotation.isObject()) {

            if (rotation.contains("x")) {
                matjson::Value rotationX = rotation["x"];
                if (!rotationX.isNull() && rotationX.isNumber()) {
                    node->setRotationX(rotationX.asDouble().unwrapOr(0));
                }
            }
            if (rotation.contains("y")) {
                matjson::Value rotationY = rotation["y"];
                if (!rotationY.isNull() && rotationY.isNumber()) {
                    node->setRotationY(rotationY.asDouble().unwrapOr(0));
                }
            }
        }

        if (rotation.isNumber()) {
            node->setRotation(rotation.asDouble().unwrapOr(0));
        }
    }
}

void UIModding::setSkew(CCNode* node, matjson::Value attributes) {
    if (attributes.contains("skew")) {
        matjson::Value skew = attributes["skew"];
        if (skew.isObject()) {
            if (skew.contains("x")) {
                matjson::Value skewX = skew["x"];
                if (skewX.isNumber()) node->setSkewX(skewX.asDouble().unwrapOr(0));
            }

            if (skew.contains("y")) {
                matjson::Value skewY = skew["y"];
                if (skewY.isNumber()) node->setSkewY(skewY.asDouble().unwrapOr(0));
            }
        }
    }
}

void UIModding::setAnchorPoint(CCNode* node, matjson::Value attributes) {
    if (attributes.contains("anchor-point")) {
        matjson::Value anchorPoint = attributes["anchor-point"];
        if (anchorPoint.isObject()) {
            if (anchorPoint.contains("x") && anchorPoint.contains("y")) {
                matjson::Value anchorPointX = anchorPoint["x"];
                matjson::Value anchorPointY = anchorPoint["y"];

                if (anchorPointX.isNumber() && anchorPointY.isNumber()) {
                    float anchorPointValueX = anchorPointX.asDouble().unwrapOr(0);
                    float anchorPointValueY = anchorPointY.asDouble().unwrapOr(0);
                    node->setAnchorPoint({anchorPointValueX, anchorPointValueY});
                }
            }
        }
    }
}

void UIModding::setContentSize(CCNode* node, matjson::Value attributes) {
    if (attributes.contains("content-size")) {
        matjson::Value contentSize = attributes["content-size"];
        if (contentSize.isObject()) {
            if (contentSize.contains("width") && contentSize.contains("height")) {
                matjson::Value contentSizeWidth = contentSize["width"];
                matjson::Value contentSizeHeight = contentSize["height"];

                if (contentSizeWidth.isNumber() && contentSizeHeight.isNumber()) {
                    float contentSizeValueWidth = contentSizeWidth.asDouble().unwrapOr(0);
                    float contentSizeValueHeight = contentSizeHeight.asDouble().unwrapOr(0);

                    node->setContentSize({contentSizeValueWidth, contentSizeValueHeight});
                }
            }
        }
    }
}

void UIModding::handleModifications(CCNode* node, matjson::Value nodeObject) {

    if (DataNode* data = typeinfo_cast<DataNode*>(node)){
        node = data->m_data;
    }

    if (nodeObject.contains("attributes")) {
        matjson::Value nodeAttributes = nodeObject["attributes"];
        if (nodeAttributes.isObject()) {
            nodeAttributes["_pack-name"] = nodeObject["_pack-name"];
            
            nodesFor(setDisablePages);
            nodesFor(setLayout);
            nodesFor(setScale);
            nodesFor(setRotation);
            nodesFor(setSkew);
            nodesFor(setAnchorPoint);
            nodesFor(setContentSize);
            nodesFor(setVisible);
            nodesFor(setIgnoreAnchorPos);
            nodesFor(setColor);
            nodesFor(setOpacity);
            nodesFor(setSprite);
            nodesFor(setSpriteFrame);
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
            nodesFor(updateLayout);
            #ifndef GEODE_IS_MACOS
            nodesFor(runAction);
            #endif
            //todo wait for fmod fixes
            nodesFor(playSound);
            nodesFor(openLink);
            nodesFor(runCallback);
        }
    }

    if (nodeObject.contains("event")) {
        matjson::Value eventVal = nodeObject["event"];
        if (eventVal.isObject()) {

            eventVal["_pack-name"] = nodeObject["_pack-name"];
            if (EventCCMenuItemSpriteExtra* button = static_cast<EventCCMenuItemSpriteExtra*>(node)) {
                forEvent(on-click, OnClick);
                forEvent(on-release, OnRelease);
                forEvent(on-activate, OnActivate);
                forEvent(on-hover, OnHover);
                forEvent(on-exit, OnExit);
            }
        }
    }

    if (nodeObject.contains("parent")) {
        matjson::Value parentVal = nodeObject["parent"];
        if (parentVal.isObject()) {
            parentVal["_pack-name"] = nodeObject["_pack-name"];
            if (CCNode* parent = node->getParent()) handleModifications(parent, parentVal);
        }
    }

    if (nodeObject.contains("children")) {
        matjson::Value childrenVal = nodeObject["children"];
        if (childrenVal.isObject()) {
            childrenVal["_pack-name"] = nodeObject["_pack-name"];

            if (childrenVal.contains("node")) {
                matjson::Value nodeChildrenVal = childrenVal["node"];
                if (nodeChildrenVal.isObject()) {
                    nodeChildrenVal["_pack-name"] = nodeObject["_pack-name"];
                    recursiveModify(node, nodeChildrenVal);
                }
            }
            if (childrenVal.contains("index")) {
                matjson::Value indexChildrenVal = childrenVal["index"];
                if (indexChildrenVal.isArray()) {
                    geode::Result<std::vector<matjson::Value>&> nodeChildrenArray = indexChildrenVal.asArray();
                    if (nodeChildrenArray.isOk()) {
                        for (matjson::Value value : nodeChildrenArray.unwrap()) {
                            if (value.isObject()) {
                                value["_pack-name"] = nodeObject["_pack-name"];

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
                        }
                    }
                }
            }
            if (childrenVal.contains("all")) {
                matjson::Value allChildrenVal = childrenVal["all"];
                if (allChildrenVal.isObject()) {
                    allChildrenVal["_pack-name"] = nodeObject["_pack-name"];

                    CCArray* children = node->getChildren();
                    if (CCArray* pageChildren = typeinfo_cast<CCArray*>(node->getUserObject("alphalaneous.pages_api/page-children"))){
                        children = pageChildren;
                    }

                    for (CCNode* node : CCArrayExt<CCNode*>(children)) {
                        handleModifications(node, allChildrenVal);
                    }
                }
            }
            if (childrenVal.contains("new")) {
                matjson::Value newChildrenVal = childrenVal["new"];
                if (newChildrenVal.isArray()) {
                    geode::Result<std::vector<matjson::Value>&> newChildrenArray = newChildrenVal.asArray();
                    
                    if (newChildrenArray.isOk()) {
                        for (matjson::Value value : newChildrenArray.unwrap()) {
                            if (value.isObject()) {
                                value["_pack-name"] = nodeObject["_pack-name"];

                                int index = 0;
                                std::string type = "Node";
                                if (value.contains("type") && value.contains("id")) {
                                    matjson::Value typeVal = value["type"];
                                    if (typeVal.isString()) {
                                        type = typeVal.asString().unwrapOr("");
                                        CCNode* newNode = nullptr;
                                        if (type == "CCSprite") {
                                            newNode = CCSprite::create();
                                        }
                                        if (type == "CCLabelBMFont") {
                                            newNode = CCLabelBMFont::create("", "chatFont.fnt");
                                        }
                                        if (type == "CCMenu") {
                                            newNode = CCMenu::create();
                                        }
                                        if (type == "CCLayerColor") {
                                            newNode = CCLayerColor::create(ccColor4B{0,0,0,0});
                                        }
                                        if (type == "CCMenuItemSpriteExtra") {
                                            newNode = CCMenuItemSpriteExtra::create(CCSprite::create(), nullptr, nullptr, nullptr);
                                        }
                                        if (type == "CCScale9Sprite") {
                                            if(value.contains("attributes")){
                                                matjson::Value attributesVal = value["attributes"];
                                                if(attributesVal.contains("sprite")){
                                                    matjson::Value spriteVal = attributesVal["sprite"];
                                                    if(spriteVal.isString()){
                                                        std::string sprite = spriteVal.asString().unwrapOr("");
                                                        newNode = CCScale9Sprite::create(sprite.c_str());
                                                    }
                                                }
                                            }
                                        }
                                        if (type == "Alert") {
                                            if (value.contains("attributes")) {
                                                matjson::Value attributesVal = value["attributes"];
                                                if (attributesVal.contains("title") && attributesVal.contains("description")) {
                                                    matjson::Value titleVal = attributesVal["title"];
                                                    matjson::Value descVal = attributesVal["description"];
                                                    if (titleVal.isString() && descVal.isString()) {
                                                        std::string title = titleVal.asString().unwrapOr("");
                                                        std::string description = descVal.asString().unwrapOr("");
                                                        std::string buttonText = "Okay";
                                                        if (attributesVal.contains("button-text")) {
                                                            matjson::Value buttonVal = attributesVal["button-text"];
                                                            if (buttonVal.isString()) {
                                                                buttonText = buttonVal.asString().unwrapOr("");
                                                            }
                                                        }
                                                        FLAlertLayer* alert = geode::createQuickPopup(title.c_str(), description, buttonText.c_str(), nullptr, nullptr, false, true);
                                                        static_cast<MyFLAlertLayer*>(alert)->setRift();
                                                        DataNode* data = DataNode::create(alert);
                                                        newNode = data;
                                                    }
                                                }
                                            }
                                        }
                                        if (type == "Popup") {
                                            std::string sprite = "GJ_square01.png";
                                            std::string title = "";
                                            float width = 60;
                                            float height = 60;

                                            if (value.contains("attributes")) {
                                                matjson::Value attributesVal = value["attributes"];
                                                
                                                
                                                if (attributesVal.contains("sprite")) {
                                                    matjson::Value spriteVal = attributesVal["sprite"];
                                                    if (spriteVal.isString()) {
                                                        sprite = spriteVal.asString().unwrapOr("");
                                                    }
                                                }
                                                if (attributesVal.contains("popup-size")) {
                                                    matjson::Value contentSize = attributesVal["popup-size"];
                                                    if (contentSize.isObject()) {
                                                        if (contentSize.contains("width") && contentSize.contains("height")) {
                                                            matjson::Value contentSizeWidth = contentSize["width"];
                                                            matjson::Value contentSizeHeight = contentSize["height"];

                                                            if (contentSizeWidth.isNumber() && contentSizeHeight.isNumber()) {
                                                                width = contentSizeWidth.asDouble().unwrapOr(0);
                                                                height = contentSizeHeight.asDouble().unwrapOr(0);
                                                            }
                                                        }
                                                    }
                                                }
                                                if (attributesVal.contains("title")) {
                                                    matjson::Value titleVal = attributesVal["title"];
                                                    if (titleVal.isString()) {
                                                        title = titleVal.asString().unwrapOr("");
                                                    }
                                                }
                                            }
                                            CustomAlert* alert = CustomAlert::create(width, height, sprite, title);
                                            DataNode* data = DataNode::create(alert);
                                            newNode = data;
                                        }
                                        if (newNode) {
                                            
                                            matjson::Value idVal = value["id"];
                                            std::string fullID;
                                            if (idVal.isString()) {
                                                std::string id = idVal.asString().unwrapOr("");
                                                std::string packName = "missing";
                                                if (nodeObject.contains("_pack-name") && nodeObject["_pack-name"].isString()) {
                                                    packName = nodeObject["_pack-name"].asString().unwrapOr("");
                                                }
                                                fullID = fmt::format("{}/{}", packName, id);

                                                newNode->setID(fullID.c_str());
                                            }

                                            if (DataNode* data = typeinfo_cast<DataNode*>(newNode)) {
                                                if (FLAlertLayer* alert = typeinfo_cast<FLAlertLayer*>(data->m_data)) {
                                                    alert->setID(fullID.c_str());
                                                }
                                                data->setID(fullID.c_str());
                                            }

                                            if (FLAlertLayer* alert = typeinfo_cast<FLAlertLayer*>(node)) {
                                                alert->m_mainLayer->removeChildByID(fullID);
                                                alert->m_mainLayer->addChild(newNode);
                                            }
                                            else {
                                                node->removeChildByID(fullID);
                                                node->addChild(newNode);
                                            }
                                            
                                            handleModifications(newNode, value);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void UIModding::doUICheck(CCNode* node) {

    std::string nodeID = node->getID();
    std::replace(nodeID.begin(), nodeID.end(), '/', '$');
    std::string path = "ui/" + nodeID + ".json";
    
	
    unsigned long fileSize = 0;
    unsigned char* buffer = CCFileUtils::sharedFileUtils()->getFileData(path.c_str(), "rb", &fileSize);    

    if (buffer && fileSize != 0) {

        std::string data = std::string(reinterpret_cast<char*>(buffer), fileSize);
        geode::Result<matjson::Value, matjson::ParseError> value = matjson::parse(data);

        if (value.isOk()) {

            matjson::Value expandedValue = value.unwrap();
            if (expandedValue.isObject()) {
                std::string fullPathStr = CCFileUtils::sharedFileUtils()->fullPathForFilename(path.c_str(), false);

                std::filesystem::path fullPath(fullPathStr);

                std::string name = fullPath.parent_path().parent_path().filename().string();
                name = utils::string::toLower(name);
                if (name == "resources") {
                    name = fullPath.parent_path().parent_path().parent_path().filename().string();
                    name = utils::string::toLower(name);
                }
                std::replace( name.begin(), name.end(), ' ', '-');
                
                expandedValue["_pack-name"] = name.substr(0, name.find_last_of("."));;

                handleModifications(node, expandedValue);
            }
            for (CCNode* node : CCArrayExt<CCNode*>(removalQueue)) {
                node->removeFromParentAndCleanup(true);
            }
            removalQueue->removeAllObjects();
        }
    }

    delete[] buffer;
}

void UIModding::startFileListeners() {
    for (FileWatcher* fw : listeners) {
        fw->stop();
    }
    listeners.clear();
    std::vector<std::string> packs = Utils::getActivePacks();
    for (std::string path : packs) {

        std::string uiPath = fmt::format("{}{}", path, "ui\\");

        FileWatcher* fw = new FileWatcher(uiPath, std::chrono::milliseconds(500));
        listeners.push_back(fw);

        std::thread thread([=](FileWatcher* self){
            self->start([=] (std::string pathToWatch, FileStatus status) -> void {
                if (!std::filesystem::is_regular_file(std::filesystem::path(pathToWatch)) && status != FileStatus::erased) {
                    return;
                }
                Loader::get()->queueInMainThread([]{
                    UIModding::get()->uiCache.clear();
                    UIModding::get()->colorCache.clear();
                    CCScene* scene = CCDirector::sharedDirector()->getRunningScene();
                    for (CCNode* node : CCArrayExt<CCNode*>(scene->getChildren())) {
                        UIModding::get()->doUICheck(node);
                    }
                });
            });
        }, fw);
        thread.detach();
    }
}



AxisAlignment UIModding::getAxisAlignment(std::string name) {
    AxisAlignment axisAlignment = AxisAlignment::Start;
    if (name == "start") {
        axisAlignment = AxisAlignment::Start;
    }
    if (name == "center") {
        axisAlignment = AxisAlignment::Center;
    }
    if (name == "end") {
        axisAlignment = AxisAlignment::End;
    }
    if (name == "even") {
        axisAlignment = AxisAlignment::Even;
    }
    if (name == "between") {
        axisAlignment = AxisAlignment::Between;
    }
    return axisAlignment;
}

void UIModding::loadNodeFiles() {
    std::vector<std::string> packs = Utils::getActivePacks();
    for (std::string path : packs) {
        std::string nodePath = fmt::format("{}{}", path, "ui\\nodes\\");
        if (std::filesystem::is_directory(nodePath)) {
            for (const auto& entry : std::filesystem::directory_iterator(nodePath)) {

                matjson::Value value;
                std::string fileName = entry.path().filename().string();
                
                std::vector<std::string> parts = utils::string::split(fileName, ".");
                std::string type = parts.at(0);

                std::string path = "ui/nodes/" + fileName;

                unsigned long fileSize = 0;
                unsigned char* buffer = CCFileUtils::sharedFileUtils()->getFileData(path.c_str(), "rb", &fileSize);    

                if (buffer && fileSize != 0) {
                    
                    std::string data = std::string(reinterpret_cast<char*>(buffer), fileSize);
                    geode::Result<matjson::Value, matjson::ParseError> valueOpt = matjson::parse(data);

                    if (valueOpt.isOk()) {
                        value = valueOpt.unwrap();
                        uiCache.insert({type, value});
                    }
                    else {
                        uiCache.insert({type, matjson::Value(nullptr)});
                    }
                }
                else {
                    uiCache.insert({type, matjson::Value(nullptr)});
                }
                delete[] buffer;
            }
        }
    }
}

void UIModding::doUICheckForType(std::string type, CCNode* node) {
    if (uiCache.contains(type)) {
        matjson::Value value = uiCache[type];
        if (!value.isNull()) {
            handleModifications(node, value);
        }
    }
}
