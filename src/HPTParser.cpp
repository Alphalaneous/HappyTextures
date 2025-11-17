#include "HPTParser.hpp"
#include "Utils.hpp"
#include "nodes/CCLabelBMFont.hpp"
#include "UIModding.hpp"
#include "Callbacks.hpp"
#include <alphalaneous.pages_api/include/PagesAPI.h>
#include "HPTCCNode.hpp"
#include <Geode/ui/GeodeUI.hpp>
#include "NodeFactory.hpp"

HPTParser& HPTParser::get() {
    static HPTParser instance;
    return instance;
}

void HPTParser::setupParser() {
    NodeFactory::get().setupNodes();

    registerNodeType("command", {"Command", false, true});
    registerAttribute("color", [](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        if (value.empty()) return;

        auto color = Utils::parseColorFromString(value).unwrapOr(ccColor4B{255, 0, 255, 255});
        auto color3B = ccColor3B{color.r, color.g, color.b};
        auto alpha = color.a;

        if (auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
            if (auto buttonNode = btn->getChildByType<ButtonSprite>(0)) {
                buttonNode->setColor(color3B);
                buttonNode->setOpacity(alpha);
            }
        }
        if (auto rgba = typeinfo_cast<CCRGBAProtocol*>(node)) {
            rgba->setColor(color3B);
            rgba->setOpacity(alpha);
        }
    }, [](CCNode* node) -> std::string {
        if (auto rgba = typeinfo_cast<CCRGBAProtocol*>(node)) {
            auto color = rgba->getColor();
            return fmt::format("rgba({}, {}, {}, {})", color.r, color.g, color.b, rgba->getOpacity());
        }
        return "";
    });

    registerAttribute("scale-x", [](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        auto xNum = numFromString<float>(value);
        if (xNum) {
            node->setScaleX(xNum.unwrap());
        }
    }, nullptr);

    registerAttribute("scale-y", [](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        auto yNum = numFromString<float>(value);
        if (yNum) {
            node->setScaleY(yNum.unwrap());
        }
    }, nullptr);

    registerAttribute("scale", [](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        auto num = numFromString<float>(value);
        if (num) {
            node->setScale(num.unwrap());
            if (auto button = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
                button->m_baseScale = num.unwrap();
            }
            return;
        }

        auto xStr = hptNode->attributes["x"];
        auto xNum = numFromString<float>(xStr);

        if (xNum) {
            node->setScaleX(xNum.unwrap());
        }

        auto yStr = hptNode->attributes["y"];
        auto yNum = numFromString<float>(yStr);

        if (yNum) {
            node->setScaleY(yNum.unwrap());
        }

        if (auto fitRes = hptNode->get("fit")) {
            auto fit = fitRes.unwrap();
            std::string fitRelative = fit.attributes["relative"];
            float scaleMultiplier = numFromString<float>(fit.attributes["multiplier"]).unwrapOr(1.f);

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
                if (auto sizeRes = fit["size"]) {
                    auto size = sizeRes.unwrap();
                    targetSize.width = numFromString<float>(size.attributes["width"]).unwrapOr(-1.f);
                    targetSize.height = numFromString<float>(size.attributes["height"]).unwrapOr(-1.f);
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
                float mulScale = scale * scaleMultiplier;
                node->setScale(mulScale);
                if (auto button = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
                    button->m_baseScale = mulScale;
                }
            }
            return;
        }
    }, [](CCNode* node) -> std::string {
        return fmt::format("{{\n x: {}\n y: {} \n}}", node->getScaleX(), node->getScaleY());
    });

    registerAttribute("rotation-x", [](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        auto xNum = numFromString<float>(value);
        if (xNum) {
            node->setRotationX(xNum.unwrap());
        }
    }, nullptr);

    registerAttribute("rotation-y", [](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        auto yNum = numFromString<float>(value);
        if (yNum) {
            node->setRotationY(yNum.unwrap());
        }
    }, nullptr);

    registerAttribute("rotation", [](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        auto num = numFromString<float>(value);
        if (num) {
            node->setScale(num.unwrap());
            return;
        }
 
        auto xStr = hptNode->attributes["x"];
        auto xNum = numFromString<float>(xStr);

        if (xNum) {
            node->setRotationX(xNum.unwrap());
        }

        auto yStr = hptNode->attributes["y"];
        auto yNum = numFromString<float>(yStr);

        if (yNum) {
            node->setRotationY(yNum.unwrap());
        }
    }, [](CCNode* node) -> std::string {
        return fmt::format("{{\n x: {}\n y: {} \n}}", node->getRotationX(), node->getRotationY());
    });

    registerAttribute("skew-x", [](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        auto xNum = numFromString<float>(value);
        if (xNum) {
            node->setSkewX(xNum.unwrap());
        }
    }, nullptr);

    registerAttribute("skew-y", [](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        auto yNum = numFromString<float>(value);
        if (yNum) {
            node->setSkewY(yNum.unwrap());
        }
    }, nullptr);

    registerAttribute("skew", [](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        auto xStr = hptNode->attributes["x"];
        auto xNum = numFromString<float>(xStr);

        if (xNum) {
            node->setSkewX(xNum.unwrap());
        }

        auto yStr = hptNode->attributes["y"];
        auto yNum = numFromString<float>(yStr);

        if (yNum) {
            node->setSkewY(yNum.unwrap());
        }
    }, [](CCNode* node) -> std::string {
        return fmt::format("{{\n x: {}\n y: {} \n}}", node->getSkewX(), node->getSkewY());
    });

    registerAttribute("anchor-x", [](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        auto xNum = numFromString<float>(value);
        if (xNum) {
            auto anchor = node->getAnchorPoint();
            anchor.x = xNum.unwrap();
            node->setAnchorPoint(anchor);
        }
    }, nullptr);

    registerAttribute("anchor-y", [](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        auto yNum = numFromString<float>(value);
        if (yNum) {
            auto anchor = node->getAnchorPoint();
            anchor.y = yNum.unwrap();
            node->setAnchorPoint(anchor);
        }
    }, nullptr);

    registerAttribute("anchor-point", [](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        auto xStr = hptNode->attributes["x"];
        auto xNum = numFromString<float>(xStr);

        auto anchor = node->getAnchorPoint();

        if (xNum) {
            anchor.x = xNum.unwrap();
        }

        auto yStr = hptNode->attributes["y"];
        auto yNum = numFromString<float>(yStr);

        if (yNum) {
            anchor.y = yNum.unwrap();
        }

        node->setAnchorPoint(anchor);
    }, [](CCNode* node) -> std::string {
        return fmt::format("{{\n x: {}\n y: {} \n}}", node->getAnchorPoint().x, node->getAnchorPoint().y);
    });

    registerAttribute("content-width", [](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        auto widthNum = numFromString<float>(value);
        if (widthNum) {
            node->setContentWidth(widthNum.unwrap());
        }
    }, nullptr);

    registerAttribute("content-height", [](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        auto heightNum = numFromString<float>(value);
        if (heightNum) {
            node->setContentHeight(heightNum.unwrap());
        }
    }, nullptr);

    registerAttribute("content-size", [](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        auto widthStr = hptNode->attributes["width"];
        auto widthNum = numFromString<float>(widthStr);

        auto size = node->getContentSize();

        if (widthNum) {
            size.width = widthNum.unwrap();
        }

        auto heightStr = hptNode->attributes["height"];
        auto heightNum = numFromString<float>(heightStr);

        if (heightNum) {
            size.height = heightNum.unwrap();
        }

        node->setContentSize(size);
    }, [](CCNode* node) -> std::string {
        return fmt::format("{{\n width: {}\n height: {} \n}}", node->getContentWidth(), node->getContentHeight());
    });

    registerAttribute("ignore-anchor-point-for-position", [](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        auto boolRes = Utils::boolFromString(value);
        if (boolRes) {
            node->ignoreAnchorPointForPosition(boolRes.unwrap());
        }
    }, [](CCNode* node) -> std::string {
        return fmt::format("{}", node->isIgnoreAnchorPointForPosition());
    });

    registerAttribute("update-layout", [](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        auto boolRes = Utils::boolFromString(value);
        if (boolRes && boolRes.unwrap()) {
            if (auto parent = node->getParent()) {
                parent->updateLayout();
            }
            return;
        }
        if (value == "self") {
            node->updateLayout();
        } else if (value == "parent") {
            if (auto parent = node->getParent()) {
                parent->updateLayout();
            }
        }
    }, nullptr);

    registerAttribute("visible", [](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        auto boolRes = Utils::boolFromString(value);
        if (boolRes) {
            node->setVisible(boolRes.unwrap());
        }
    }, [](CCNode* node) -> std::string {
        return fmt::format("{}", node->isVisible());
    });

    registerAttribute("opacity", [](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        auto floatRes = numFromString<float>(value);
        if (floatRes) {
            auto opacity = floatRes.unwrap();
            if (auto node1 = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
                node1->setOpacity(opacity);
                if (auto node2 = node1->getChildByType<ButtonSprite>(0)) {
                    node2->setOpacity(opacity);
                }
            }

            if (auto nodeRGBA = typeinfo_cast<CCNodeRGBA*>(node)) {
                nodeRGBA->setOpacity(opacity);
            }
            if (auto layerRGBA = typeinfo_cast<CCLayerRGBA*>(node)) {
                layerRGBA->setOpacity(opacity);
            }
        }
    }, nullptr);

    registerAttribute("sprite", [this](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        std::string spriteName;
        CCSprite* spr = nullptr;

        if (!value.empty()) {
            spriteName = value;
        }
        else {
            auto spriteRes = hptNode->get("sprite");
            if (spriteRes) {
                auto sprite = spriteRes.unwrap();

                auto randomObjectRes = sprite["random"];
                if (randomObjectRes) {
                    auto randomObject = randomObjectRes.unwrap();
                    std::string mode = "load";
                    std::string modeAttr = randomObject.attributes["mode"];
                    if (!modeAttr.empty()) mode = modeAttr;
                    std::string id = randomObject.attributes["id"];
                    std::string path = randomObject.attributes["path"];
                    std::string spriteListStr = randomObject.attributes["sprites"];
                    size_t pos = 0;
                    auto spriteList = parseArguments(hptNode, spriteListStr, pos, '[', ']', 0);

                    if (mode == "load" || id.empty()) {
                        std::vector<std::string> validSprites = UIModding::get()->generateValidSprites(path, spriteList);
                        if (!validSprites.empty()) spriteName = validSprites.at(Utils::getRandomNumber(0, validSprites.size()-1));
                    }
                    else if (mode == "session") {
                        if (!UIModding::get()->randomSprites.contains(id)) {
                            std::vector<std::string> validSprites = UIModding::get()->generateValidSprites(path, spriteList);
                            if (!validSprites.empty()) UIModding::get()->randomSprites[id] = validSprites.at(Utils::getRandomNumber(0, validSprites.size()-1));
                        }
                        spriteName = UIModding::get()->randomSprites[id];
                    }
                }
            }
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
            auto infoRes = hptNode->get("button-info");
            if (infoRes) {
                auto info = infoRes.unwrap();
                if (info.attributes.contains(std::string_view("type"))) {
                    std::string type = info.attributes["type"];

                    if (type == "labeled") {
                        std::string caption, font = "bigFont.fnt", texture = "GJ_button_01.png";
                        int width = 30;
                        float height = 30, scale = 1;
                        bool absolute = false;

                        if (!info.attributes["text"].empty()) caption = info.attributes["text"];
                        if (!info.attributes["font"].empty()) font = info.attributes["font"];
                        if (!info.attributes["sprite"].empty()) texture = info.attributes["sprite"];
                        if (auto boolRes = Utils::boolFromString(info.attributes["absolute"])) absolute = boolRes.unwrap();
                        if (auto floatRes = numFromString<float>(info.attributes["width"])) width = floatRes.unwrap();
                        if (auto floatRes = numFromString<float>(info.attributes["height"])) height = floatRes.unwrap();
                        if (auto floatRes = numFromString<float>(info.attributes["scale"])) scale = floatRes.unwrap();

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
    }, nullptr);

    registerAttribute("text", [this](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        if (value.empty()) return;

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
            textObject->setString(std::string(value).c_str());

            auto textFields = myTextObject->m_fields.self();

            if (textFields->m_isLimited) {
                textObject->limitLabelWidth(
                    textFields->m_limitWidth,
                    textFields->m_limitDefaultScale,
                    textFields->m_limitMinScale
                );
            }
        }
    }, [](CCNode* node) -> std::string {
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
            return textObject->getString();
        }
        return "";
    });

    registerAttribute("font", [this](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        if (value.empty()) return;

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

        if (textObject && value.ends_with(".fnt")) {
            auto myTextObject = static_cast<MyCCLabelBMFont*>(textObject);
            auto& fields = myTextObject->m_fields;

            auto font = std::string(value);

            if (FNTConfigLoadFile(font.c_str())) {
                textObject->setFntFile(font.c_str());

                if (fields->m_isLimited) {
                    textObject->limitLabelWidth(fields->m_limitWidth, fields->m_limitDefaultScale, fields->m_limitMinScale);
                }
            }
        }
    }, [](CCNode* node) -> std::string {
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
            return textObject->getFntFile();
        }
        return "";
    });

    registerAttribute("base-scale", [this](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        auto floatRes = numFromString<float>(value);
        if (floatRes) {
            if (auto button = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
                button->m_baseScale = floatRes.unwrap();
            }
        }
    }, [](CCNode* node) -> std::string {
        if (auto button = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
            return fmt::format("{}", button->m_baseScale);
        }
        return "";
    });

    registerAttribute("scale-multiplier", [this](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        auto floatRes = numFromString<float>(value);
        if (floatRes) {
            if (auto button = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
                button->m_scaleMultiplier = floatRes.unwrap();
            }
        }
    }, [](CCNode* node) -> std::string {
        if (auto button = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
            return fmt::format("{}", button->m_scaleMultiplier);
        }
        return "";
    });

    registerAttribute("remove", [this](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        auto boolRes = Utils::boolFromString(value);
        if (boolRes && boolRes.unwrap()) {
            UIModding::get()->removalQueue.push_back(node);
        }
    }, nullptr);

    registerAttribute("position-x", [this](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        auto xNum = numFromString<float>(value);
        if (xNum) {
            node->setPositionX(xNum.unwrap());
        }
    }, nullptr);

    registerAttribute("position-y", [this](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        auto yNum = numFromString<float>(value);
        if (yNum) {
            node->setPositionY(yNum.unwrap());
        }
    }, nullptr);

    registerAttribute("position", [this](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        auto xStr = hptNode->attributes["x"];
        auto xNum = numFromString<float>(xStr);

        auto newPos = CCPoint{0, 0};
        auto pos = node->getPosition();
        auto parent = node->getParent();
        auto nodeSize = parent ? parent->getContentSize() : CCDirector::get()->getWinSize();

        if (xNum) {
            newPos.x = xNum.unwrap();
        }

        auto yStr = hptNode->attributes["y"];
        auto yNum = numFromString<float>(yStr);

        if (yNum) {
            newPos.y = yNum.unwrap();
        }

        std::string anchorTo = "";

        if (auto anchorRes = hptNode->get("anchor")) {
            auto anchor = anchorRes.unwrap();
            anchorTo = anchor.attributes["to"];
            std::string relative = anchor.attributes["relative"];
            if (relative == "screen") nodeSize = CCDirector::get()->getWinSize();
            else if (relative == "parent") nodeSize = parent->getContentSize();
        }
        else if (hptNode->attributes.contains(std::string_view("anchor"))) {
            anchorTo = hptNode->attributes["anchor"];
        }

        if (!anchorTo.empty()) {
            if (anchorTo == "top-left") {
                newPos.y += nodeSize.height;
            } else if (anchorTo == "top-center") {
                newPos.x += nodeSize.width / 2;
                newPos.y += nodeSize.height;
            } else if (anchorTo == "top-right") {
                newPos.x += nodeSize.width;
                newPos.y += nodeSize.height;
            } else if (anchorTo == "center-left") {
                newPos.y += nodeSize.height / 2;
            } else if (anchorTo == "center") {
                newPos.x += nodeSize.width / 2;
                newPos.y += nodeSize.height / 2;
            } else if (anchorTo == "center-right") {
                newPos.x += nodeSize.width;
                newPos.y += nodeSize.height / 2;
            } else if (anchorTo == "bottom-center") {
                newPos.x += nodeSize.width / 2;
            } else if (anchorTo == "bottom-right") {
                newPos.x += nodeSize.width;
            } else if (anchorTo == "self") {
                newPos += pos;
            }
        }
        node->setPosition(newPos);
    }, [](CCNode* node) -> std::string {
        return fmt::format("{{\n x: {}\n y: {} \n}}", node->getPositionX(), node->getPositionY());
    });

    registerAttribute("flip-x", [this](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        if (auto sprite = typeinfo_cast<CCSprite*>(node)) {
            auto xRes = Utils::boolFromString(value);
            if (xRes) {
                sprite->setFlipX(xRes.unwrap());
            }
        }
    }, nullptr);

    registerAttribute("flip-y", [this](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        if (auto sprite = typeinfo_cast<CCSprite*>(node)) {
            auto yRes = Utils::boolFromString(value);
            if (yRes) {
                sprite->setFlipY(yRes.unwrap());
            }
        }
    }, nullptr);

    registerAttribute("flip", [this](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        if (auto sprite = typeinfo_cast<CCSprite*>(node)) {

            bool flipX = sprite->isFlipX();
            bool flipY = sprite->isFlipY();

            auto xRes = Utils::boolFromString(hptNode->attributes["x"]);
            if (xRes) {
                flipX = xRes.unwrap();
            }

            auto yRes = Utils::boolFromString(hptNode->attributes["y"]);
            if (yRes) {
                flipY = yRes.unwrap();
            }

            sprite->setFlipX(flipX);
            sprite->setFlipY(flipY);
        }
    }, [](CCNode* node) -> std::string {
        if (auto sprite = typeinfo_cast<CCSprite*>(node)) {
            return fmt::format("{{\n x: {}\n y: {}\n}}", sprite->isFlipX(), sprite->isFlipY());
        }
        return "";
    });

    registerAttribute("blending", [this](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        if (!hptNode->attributes.contains(std::string_view("source")) || !hptNode->attributes.contains(std::string_view("destination"))) return;

        std::string source = hptNode->attributes["source"];
        std::string destination = hptNode->attributes["destination"];

        unsigned int src = Utils::stringToBlendingMode(source);
        unsigned int dst = Utils::stringToBlendingMode(destination);

        if (src != -1 && dst != -1) {
            if (CCBlendProtocol* blendable = typeinfo_cast<CCBlendProtocol*>(node)) {
                blendable->setBlendFunc({src, dst});
            }
        }
    }, [](CCNode* node) -> std::string {
        return ""; //TODO BLENDING MODE TO STRING
    });

    registerAttribute("z-order", [this](CCNode* node, std::shared_ptr<HPTNode>, std::string_view value) {
        auto zOrder = numFromString<float>(value);
        if (zOrder) {
            node->setZOrder(zOrder.unwrap());
        }
    }, [](CCNode* node) -> std::string {
        return fmt::format("{}", node->getZOrder());
    });

    registerAttribute("show", [this](CCNode* node, std::shared_ptr<HPTNode>, std::string_view value) {
        if (FLAlertLayer* alert = typeinfo_cast<FLAlertLayer*>(node)) {
            auto boolRes = Utils::boolFromString(value);
            if (boolRes) {
                if (boolRes.unwrap()) alert->show();
                else alert->keyBackClicked();
            }
        }
    }, nullptr);

    registerAttribute("scroll-to-top", [this](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        if (auto scrollLayer = typeinfo_cast<geode::ScrollLayer*>(node)) {
            auto boolRes = Utils::boolFromString(value);
            if (boolRes && boolRes.unwrap()) {
                scrollLayer->scrollToTop();
            }
        }
    }, nullptr);

    registerAttribute("layout", [this](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        Layout* layout;

        if (node->getLayout()) {
            auto boolRes = Utils::boolFromString(hptNode->attributes["remove"]);
            if (boolRes && boolRes.unwrap()) {
                node->setLayout(nullptr);
            }
            else {
                layout = node->getLayout();
            }
        } else {
            layout = AxisLayout::create(Axis::Row);
            node->setLayout(layout);
        }

        if (auto simpleAxisLayout = typeinfo_cast<SimpleAxisLayout*>(layout)) {
            std::string axis = hptNode->attributes["axis"];
            if (axis == "row") {
                simpleAxisLayout->setAxis(Axis::Row);
            } else if (axis == "column") {
                simpleAxisLayout->setAxis(Axis::Column);
            }
                
            auto flipAxisRes = Utils::boolFromString(hptNode->attributes["flip-axis"]);
            if (flipAxisRes) {
                auto flipAxis = flipAxisRes.unwrap();
                simpleAxisLayout->setMainAxisDirection(flipAxis ? AxisDirection::RightToLeft : AxisDirection::LeftToRight);
            }

            auto ignoreInvisibleRes = Utils::boolFromString(hptNode->attributes["ignore-invisible"]);
            if (ignoreInvisibleRes) {
                simpleAxisLayout->ignoreInvisibleChildren(ignoreInvisibleRes.unwrap());
            }

            auto flipCrossAxisRes = Utils::boolFromString(hptNode->attributes["flip-cross-axis"]);
            if (flipCrossAxisRes) {
                auto flipCrossAxis = flipCrossAxisRes.unwrap();
                simpleAxisLayout->setCrossAxisDirection(flipCrossAxis ? AxisDirection::RightToLeft : AxisDirection::LeftToRight);
            }
            
            auto gapRes = numFromString<float>(hptNode->attributes["gap"]);
            if (gapRes) {
                auto gap = gapRes.unwrap();
                simpleAxisLayout->setGap(gap);
            }

            std::string axisAlignmentStr = hptNode->attributes["axis-alignment"];
            if (!axisAlignmentStr.empty()) {
                MainAxisAlignment axisAlignment = UIModding::get()->getSimpleMainAxisAlignment(axisAlignmentStr);
                simpleAxisLayout->setMainAxisAlignment(axisAlignment);
            }

            std::string crossAxisAlignmentStr = hptNode->attributes["cross-axis-alignment"];
            if (!crossAxisAlignmentStr.empty()) {
                CrossAxisAlignment axisAlignment = UIModding::get()->getSimpleCrossAxisAlignment(crossAxisAlignmentStr);
                simpleAxisLayout->setCrossAxisAlignment(axisAlignment);
            }
        }

        if (auto axisLayout = typeinfo_cast<AxisLayout*>(layout)) {
            std::string axis = hptNode->attributes["axis"];
            if (axis == "row") {
                axisLayout->setAxis(Axis::Row);
            } else if (axis == "column") {
                axisLayout->setAxis(Axis::Column);
            }

            auto flipAxisRes = Utils::boolFromString(hptNode->attributes["flip-axis"]);
            if (flipAxisRes) {
                auto flipAxis = flipAxisRes.unwrap();
                axisLayout->setAxisReverse(flipAxis);
            }

            auto ignoreInvisibleRes = Utils::boolFromString(hptNode->attributes["ignore-invisible"]);
            if (ignoreInvisibleRes) {
                auto ignoreInvisible = flipAxisRes.unwrap();
                axisLayout->ignoreInvisibleChildren(ignoreInvisible);
            }

            auto flipCrossAxisRes = Utils::boolFromString(hptNode->attributes["flip-cross-axis"]);
            if (flipCrossAxisRes) {
                auto flipCrossAxis = flipCrossAxisRes.unwrap();
                axisLayout->setCrossAxisReverse(flipCrossAxis);
            }

            auto autoScaleRes = Utils::boolFromString(hptNode->attributes["auto-scale"]);
            if (autoScaleRes) {
                auto autoScale = autoScaleRes.unwrap();
                axisLayout->setAutoScale(autoScale);
            }

            auto growCrossAxisRes = Utils::boolFromString(hptNode->attributes["grow-cross-axis"]);
            if (growCrossAxisRes) {
                auto growCrossAxis = growCrossAxisRes.unwrap();
                axisLayout->setGrowCrossAxis(growCrossAxis);
            }

            auto allowCrossAxisOverflowRes = Utils::boolFromString(hptNode->attributes["allow-cross-axis-overflow"]);
            if (allowCrossAxisOverflowRes) {
                auto allowCrossAxisOverflow = allowCrossAxisOverflowRes.unwrap();
                axisLayout->setCrossAxisOverflow(allowCrossAxisOverflow);
            }

            auto gapRes = numFromString<float>(hptNode->attributes["gap"]);
            if (gapRes) {
                auto gap = gapRes.unwrap();
                axisLayout->setGap(gap);
            }

            std::string axisAlignmentStr = hptNode->attributes["axis-alignment"];
            if (!axisAlignmentStr.empty()) {
                AxisAlignment axisAlignment = UIModding::get()->getAxisAlignment(axisAlignmentStr);
                axisLayout->setAxisAlignment(axisAlignment);
            }

            std::string crossAxisAlignmentStr = hptNode->attributes["cross-axis-alignment"];
            if (!crossAxisAlignmentStr.empty()) {
                AxisAlignment axisAlignment = UIModding::get()->getAxisAlignment(crossAxisAlignmentStr);
                axisLayout->setCrossAxisAlignment(axisAlignment);
            }

            std::string crossAxisLineAlignmentStr = hptNode->attributes["cross-axis-line-alignment"];
            if (!crossAxisLineAlignmentStr.empty()) {
                AxisAlignment axisAlignment = UIModding::get()->getAxisAlignment(crossAxisLineAlignmentStr);
                axisLayout->setCrossAxisLineAlignment(axisAlignment);
            }
        }

        node->updateLayout();
    }, [](CCNode* node) -> std::string {
        return ""; //TODO LAYOUT TO EXPORT
    });

    registerAttribute("pages", [this](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        if (auto menu = typeinfo_cast<CCMenu*>(node)) {
            auto boolRes = Utils::boolFromString(hptNode->attributes["enabled"]);
            if (boolRes) {
                PagesAPI::enablePages(menu, boolRes.unwrap());
            }
            std::string orientationStr = hptNode->attributes["orientation"];
            if (!orientationStr.empty()) {
                PageOrientation orientation;
                if (orientationStr == "vertical") orientation = PageOrientation::VERTICAL;
                else if (orientationStr == "horizontal") orientation = PageOrientation::HORIZONTAL;
                PagesAPI::setOrientation(menu, orientation);
            }

            auto elementCountRes = numFromString<int>(hptNode->attributes["element-count"]);
            if (elementCountRes) {
                int elementCount = elementCountRes.unwrap();
                PagesAPI::setElementCount(menu, elementCount);
            }

            auto maxSizeRes = numFromString<float>(hptNode->attributes["max-size"]);
            if (maxSizeRes) {
                float maxSize = maxSizeRes.unwrap();
                PagesAPI::setMax(menu, maxSize);
            }

            auto buttonScaleRes = numFromString<float>(hptNode->attributes["button-scale"]);
            if (buttonScaleRes) {
                float buttonScale = buttonScaleRes.unwrap();
                PagesAPI::setButtonScale(menu, buttonScale);
            }
        }
    }, [](CCNode* node) -> std::string {
        return ""; //TODO MOVE TO API;
    });

    registerAttribute("on-click", [this](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        reinterpret_cast<HPTCCNode*>(node)->setOnClick(hptNode);
    }, nullptr, true);

    registerAttribute("on-release", [this](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        reinterpret_cast<HPTCCNode*>(node)->setOnRelease(hptNode);
    }, nullptr, true);

    registerAttribute("on-activate", [this](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        reinterpret_cast<HPTCCNode*>(node)->setOnActivate(hptNode);
    }, nullptr, true);

    registerAttribute("on-hover", [this](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        reinterpret_cast<HPTCCNode*>(node)->setOnHover(hptNode);
    }, nullptr, true);

    registerAttribute("on-exit", [this](CCNode* node, std::shared_ptr<HPTNode> hptNode, std::string_view value) {
        reinterpret_cast<HPTCCNode*>(node)->setOnExit(hptNode);
    }, nullptr, true);

    registerCommand("open-level", [] (CCNode* node, std::shared_ptr<HPTNode> hptNode, const std::vector<std::string_view>& args) {
        if (args.empty()) return;
        auto idRes = numFromString<int>(args[0]);
        if (!idRes) return;
        int id = idRes.unwrap();
        auto searchObject = GJSearchObject::create(SearchType::Type19, fmt::format("{}&gameVersion=22", id));
        auto scene = LevelBrowserLayer::scene(searchObject);
        CCDirector::get()->pushScene(CCTransitionFade::create(0.5f, scene));
    });

    registerCommand("open-profile", [] (CCNode* node, std::shared_ptr<HPTNode> hptNode, const std::vector<std::string_view>& args) {
        if (args.empty()) return;
        auto idRes = numFromString<int>(args[0]);
        if (!idRes) return;
        int id = idRes.unwrap();
        ProfilePage::create(id, false)->show();
    });

    registerCommand("open-mod", [] (CCNode* node, std::shared_ptr<HPTNode> hptNode, const std::vector<std::string_view>& args) {
        if (args.empty()) return;
        (void) openInfoPopup(std::string(args[0]));
    });

    registerCommand("open-link", [] (CCNode* node, std::shared_ptr<HPTNode> hptNode, const std::vector<std::string_view>& args) {
        if (args.empty()) return;
        Utils::openURLSafe(std::string(args[0]));
    });
    
    registerCommand("play-sound", [] (CCNode* node, std::shared_ptr<HPTNode> hptNode, const std::vector<std::string_view>& args) {
        if (args.empty()) return;
        log::info("attr: {}", hptNode->attributes);

        float speed = numFromString<float>(hptNode->attributes["speed"]).unwrapOr(1);
        float volume = numFromString<float>(hptNode->attributes["volume"]).unwrapOr(1);
        float pitch = numFromString<float>(hptNode->attributes["pitch"]).unwrapOr(1);
        bool fft = Utils::boolFromString(hptNode->attributes["fast-fourier-transform"]).unwrapOr(false);
        bool reverb = Utils::boolFromString(hptNode->attributes["reverb"]).unwrapOr(false);
        int start = numFromString<int>(hptNode->attributes["start"]).unwrapOr(0);
        int end = numFromString<int>(hptNode->attributes["end"]).unwrapOr(0);
        int fadeIn = numFromString<int>(hptNode->attributes["fade-in"]).unwrapOr(0);
        int fadeOut = numFromString<int>(hptNode->attributes["fade-out"]).unwrapOr(0);
        bool loop = Utils::boolFromString(hptNode->attributes["loop"]).unwrapOr(false);

        std::string soundPath = UIModding::get()->getSound(args[0]);
        if (!soundPath.empty()) {
            FMODAudioEngine::sharedEngine()->resumeAllAudio();
            FMODAudioEngine::sharedEngine()->playEffectAdvanced(
                soundPath, speed, 0, volume, pitch, fft, reverb, start, end, fadeIn, fadeOut, loop, 0, false, true, 0, 0, 0, 0
            );
        }
    });

    registerCommand("run-action", [] (CCNode* node, std::shared_ptr<HPTNode> hptNode, const std::vector<std::string_view>& args) {

    });

    registerCommand("stop-action", [] (CCNode* node, std::shared_ptr<HPTNode> hptNode, const std::vector<std::string_view>& args) {

    });

    registerCommand("run-callback", [] (CCNode* node, std::shared_ptr<HPTNode> hptNode, const std::vector<std::string_view>& args) {
        if (args.size() < 2) return;

        const auto& className = args[0];
        const auto& methodName = args[1];

        auto& callbacks = Callbacks::get()->m_callbacks;
        if (auto classIt = callbacks.find(className); classIt != callbacks.end()) {
            auto& methodMap = classIt->second;
            if (auto methodIt = methodMap.find(methodName); methodIt != methodMap.end()) {
                auto& [target, selector] = methodIt->second;
                (target->*selector)(Callbacks::get()->getDummyButton());
            }
        }
    });

    registerCommand("run-schedule", [] (CCNode* node, std::shared_ptr<HPTNode> hptNode, const std::vector<std::string_view>& args) {

    });

    registerCommand("stop-schedule", [] (CCNode* node, std::shared_ptr<HPTNode> hptNode, const std::vector<std::string_view>& args) {

    });

    registerCommand("for-all", [] (CCNode* node, std::shared_ptr<HPTNode> hptNode, const std::vector<std::string_view>& args) {

    });

    registerCommand("add-node", [] (CCNode* node, std::shared_ptr<HPTNode> hptNode, const std::vector<std::string_view>& args) {
        if (args.size() < 2) return;
        
        const auto& className = args[0];
        const auto& nodeID = args[1];

        if (auto handler = NodeFactory::get().getNodeHandler(className)) {
            std::string fullNodeID = fmt::format("{}/{}", hptNode->packName, nodeID);
            if (auto oldNode = node->getChildByID(fullNodeID)) {
                oldNode->removeFromParent();
            }
            auto newNode = handler(hptNode);
            newNode->setID(fullNodeID);
            hptNode->targetNode = newNode;
            hptNode->typeDef = HPTNodeTypeDef{"Default", false, false};
            node->addChild(newNode);
            hptNode->reparse();
        }
    });
}

void HPTParser::registerAttribute(std::string_view name, AttributeHandler handler, AttributeExportHandler exportHandler, bool skipParsing) {
    registerNodeType(name, {std::string(name), true, skipParsing});
    m_attributeHandlers.emplace(name, handler);
}

void HPTParser::registerCommand(std::string_view name, CommandHandler handler) {
    m_commandHandlers.emplace(name, handler);
}

void HPTParser::registerNodeType(std::string_view name, const HPTNodeTypeDef& def) {
    m_nodeTypes.emplace(name, def);
}

HPTNodeTypeDef HPTParser::getNodeType(std::string_view name) {
    auto it = m_nodeTypes.find(name);
    return it != m_nodeTypes.end() ? it->second : HPTNodeTypeDef{"Default", false, false};
}

std::shared_ptr<HPTNode> HPTParser::parse(std::string_view src, std::string_view packName, std::string_view identifier, CCNode* rootNode, HPTNode* originalNode) {
    size_t pos = 0;

    auto root = std::make_shared<HPTNode>();
    root->identifier = identifier;
    root->selector = identifier;
    root->targetNode = rootNode;
    root->typeDef = HPTNodeTypeDef{"Root", false, false};
    root->packName = packName;
    if (originalNode) {
        root->parent = originalNode->parent;
        root->rowOffset = originalNode->rowOffset;
        root->skippedAttributes = originalNode->skippedAttributes;
    }

    reinterpret_cast<HPTCCNode*>(rootNode)->setOwner(root);

    while (pos < src.size()) {
        skipWhitespace(src, pos);
        parseBlock(root, src, pos, rootNode);
        auto node = parseNode(src, root, pos, rootNode);
        root->children.push_back(node);
    }
    return root;
}

ErrorInfo HPTParser::getLineColFromPos(std::string_view src, size_t pos) {
    if (pos > src.size()) pos = src.size();

    int lineNumber = 1;
    for (size_t i = 0; i < pos; i++) {
        if (src[i] == '\n') lineNumber++;
    }

    size_t start = src.rfind('\n', pos);
    if (start == std::string::npos) start = 0;
    else start += 1;

    size_t end = src.find('\n', pos);
    if (end == std::string::npos) end = src.size();

    std::string_view line = src.substr(start, end - start);
    int column = static_cast<int>(pos - start + 1);

    int lineSize = line.size();
    line = Utils::trim(line);
    int offset = lineSize - line.size();

    return { lineNumber, column, column - offset, std::string(line)};
}

std::shared_ptr<HPTNode> HPTParser::parseNode(std::string_view src, std::shared_ptr<HPTNode> parent, size_t& pos, CCNode* parentNode) {
    auto node = std::make_shared<HPTNode>();
    node->parent = parent.get();
    node->rowOffset = parent->rowOffset;
    node->identifier = parent->identifier;
    node->selector = parseSelector(parent, src, pos);
    node->targetNode = resolveSelector(parentNode, node->selector);
    node->typeDef = getNodeType(node->selector);
    node->packName = parent->packName;

    skipWhitespace(src, pos);
    if (matchChar(src, pos, ':')) {
        skipWhitespace(src, pos);
        if (matchChar(src, pos, '{')) {
            parseBlock(node, src, pos, node->targetNode);
        }
        else if (matchChar(src, pos, '^')) {
            std::string_view value = resolveValue(node, parseValue(parent, src, pos));
            size_t newPos = 0;
            if (matchChar(value, newPos, '{')) {
                parseBlock(node, value, newPos, node->targetNode);
            }
        }
    }
    else {
        skip(parent, src, pos, parentNode);
    }
    
    return node;
}

void HPTParser::parseBlock(std::shared_ptr<HPTNode> node, std::string_view src, size_t& pos, CCNode* parentNode) {
    size_t contentStart = pos;

    while (pos < src.size()) {
        skipWhitespace(src, pos);

        if (matchChar(src, pos, '}')) {
            size_t contentEnd = pos - 1;
            if (contentEnd >= contentStart) {
                node->data = src.substr(contentStart, contentEnd - contentStart);
            } else {
                node->data.clear();
            }
            break;
        }
        parseEntry(node, src, pos, parentNode);
    }
}

void HPTParser::parseEntry(std::shared_ptr<HPTNode> node, std::string_view src, size_t& pos, CCNode* parentNode) {
    skipWhitespace(src, pos);
    if (pos >= src.size()) return;

    if (node->typeDef.skipParsing) {
        skipBlock(src, pos, 1, false);
        return;
    }
    
    char c = src[pos];

    if (!node->typeDef.isAttribute) {
        if (c == '#' || c == '.' || c == '?') {
            handleNode(node, src, pos, parentNode);
        } else if (c == '!') {
            handleCommand(node, src, pos, parentNode);
        }
    }

    if (c == '^') {
        handleVariable(node, src, pos, parentNode);
    } else {
        handleAttribute(node, src, pos, parentNode);
    }
}

void HPTParser::handleNode(std::shared_ptr<HPTNode> node, std::string_view src, size_t& pos, CCNode* parentNode) {
    auto child = parseNode(src, node, pos, parentNode);
    if (node->typeDef.isAttribute) return;
    node->children.push_back(child);
}

void HPTParser::handleCommand(std::shared_ptr<HPTNode> node, std::string_view src, size_t& pos, CCNode* parentNode) {
    if (node->typeDef.isAttribute) return;
    std::string fullCommand = parseSelector(node, src, pos);
    std::string command = fullCommand.substr(1);
    std::vector<std::string_view> args;
    skipWhitespace(src, pos);
    if (matchChar(src, pos, '(')) {
        args = parseArguments(node, src, pos, '(', ')', 1);
    }

    auto child = std::make_shared<HPTNode>();
    child->parent = node.get();
    child->identifier = node->identifier;
    child->selector = fullCommand;
    child->targetNode = parentNode;
    child->packName = node->packName;
    child->typeDef = getNodeType("command");

    skipWhitespace(src, pos);
    if (matchChar(src, pos, ':')) {
        log::info("here");
        skipWhitespace(src, pos);
        if (matchChar(src, pos, '{')) {
            log::info("skip? {}", child->typeDef.skipParsing);
            parseBlock(child, src, pos, parentNode);
            node->children.push_back(child);
        }
    }
    else {
        skip(node, src, pos, parentNode);
    }
    runCommand(child, command, args);
}

void HPTParser::handleVariable(std::shared_ptr<HPTNode> node, std::string_view src, size_t& pos, CCNode* parentNode) {
    ++pos;
    std::string varName = parseSelector(node, src, pos);
    skipWhitespace(src, pos);
    if (matchChar(src, pos, ':')) {
        skipWhitespace(src, pos);
        if (matchChar(src, pos, '{')) {
            size_t start = --pos;
            skipBlock(src, pos, 0);
            std::string_view value = src.substr(start, pos - start);
            setVar(node, varName, value);
        }
        else {
            std::string value = parseValue(node, src, pos);
            setVar(node, varName, value);
        }
    }
    else {
        skip(node, src, pos, parentNode);
    }
}

void HPTParser::handleAttribute(std::shared_ptr<HPTNode> node, std::string_view src, size_t& pos, CCNode* parentNode) {
    std::string key = parseSelector(node, src, pos);
    
    size_t prePos = pos;
    skipWhitespace(src, pos);
    if (matchChar(src, pos, ':')) {
        skipWhitespace(src, pos);
        if (matchChar(src, pos, '{')) {
            auto child = std::make_shared<HPTNode>();
            child->parent = node.get();
            child->identifier = node->identifier;
            child->selector = key;
            child->skip = node->skip;

            if (node->typeDef.isAttribute) {
                child->typeDef = node->typeDef;
            }
            else {
                child->typeDef = getNodeType(key);
            }
            child->packName = node->packName;
            if (child->typeDef.skipParsing) {
                child->rowOffset += getLineColFromPos(src, prePos - 1).row - 1;
            }
            if (child->typeDef.isAttribute) {
                if (node->targetNode) {
                    child->targetNode = node->targetNode;
                    if (std::find(node->skippedAttributes.begin(), node->skippedAttributes.end(), key) != node->skippedAttributes.end()) {
                        child->skip = true;
                    }
                    parseBlock(child, src, pos, node->targetNode);
                    applyAttribute(child, key, "");
                }
            }
            else {
                child->targetNode = resolveSelector(parentNode, key);
                parseBlock(child, src, pos, child->targetNode);
                node->children.push_back(child);
            }
        } else if (matchChar(src, pos, '^')) {
            std::string orig = parseValue(node, src, pos);
            std::string value = std::string(key) + ":" + std::string(resolveValue(node, orig));
            size_t newPos = 0;
            parseEntry(node, value, newPos, node->targetNode);
        } else {
            std::string orig = parseValue(node, src, pos);
            node->attributes.emplace(key, orig);
            log::info("orig: {}", orig);
            if (node->targetNode) applyAttribute(node, key, orig);
        }
    }
    else {
        skip(node, src, pos, parentNode);
    }
}

std::string_view HPTParser::resolveValue(std::shared_ptr<HPTNode> node, std::string_view rawValue) {
    Result<std::string_view> res = getVar(node, rawValue);
    return res ? res.unwrap() : rawValue;
}

std::string HPTParser::parseSelector(std::shared_ptr<HPTNode> node, std::string_view src, size_t& pos) {
    skipWhitespace(src, pos);
    std::string sel;
    while (pos < src.size()) {
        char c = src[pos];
        if (c == ':' || c == '{' || c == '}' || c == '(' || c == ')') break;
        sel.push_back(c);
        ++pos;
    }
    utils::string::trimIP(sel);
    return process(node, sel);
}

std::string HPTParser::parseValue(std::shared_ptr<HPTNode> node, std::string_view src, size_t& pos) {
    std::string val = "";

    skipWhitespace(src, pos);
    if (pos >= src.size()) return val;

    size_t startPos = pos;

    if ((src[pos] == '"' || src[pos] == '\'') && src.find(src[pos], pos + 1) != std::string::npos) {
        char quoteChar = src[pos++];
        while (pos < src.size()) {
            char c = src[pos];
            if (c == '\\') {
                if (pos + 1 < src.size()) {
                    char next = src[pos + 1];
                    if (next == quoteChar || next == '\\') {
                        val.push_back(next);
                        pos += 2;
                        continue;
                    }
                }
                val.push_back(c);
                ++pos;
                continue;
            }
            if (c == quoteChar) {
                ++pos;
                break;
            }
            val.push_back(c);
            ++pos;
        }
    } else {
        while (pos < src.size()) {
            char c = src[pos];
            if (c == '\n' || c == '\r' || c == '}') break;
            val.push_back(c);
            ++pos;
        }
        val = process(node, val);
    }

    utils::string::trimIP(val);
    return val;
}

void HPTParser::skipWhitespace(std::string_view src, size_t& pos) {
    while (pos < src.size() && std::isspace((unsigned char)src[pos])) ++pos;
}

int numDigits(int number) {
    int digits = 0;
    if (number < 0) digits = 1;
    while (number) {
        number /= 10;
        digits++;
    }
    return digits;
}

void HPTParser::skipBlock(std::string_view src, size_t& pos, int depth, bool shouldConsumeClosing) {
    bool inQuote = false;
    char quoteChar = '\0';

    while (pos < src.size()) {
        char c = src[pos];

        if (inQuote) {
            if (c == '\\' && pos + 1 < src.size()) {
                pos += 2;
                continue;
            }
            if (c == quoteChar) {
                inQuote = false;
            }
            ++pos;
            continue;
        }

        if (c == '"' || c == '\'') {
            inQuote = true;
            quoteChar = c;
            ++pos;
            continue;
        }

        if (c == '{') {
            ++depth;
        } 
        else if (c == '}') {
            --depth;
            if (depth == 0) {
                if (shouldConsumeClosing) ++pos;
                break;
            }
        }

        ++pos;
    }
}

void HPTParser::skip(std::shared_ptr<HPTNode> parent, std::string_view src, size_t& pos, CCNode* parentNode) {
    skipWhitespace(src, pos);

    if (matchChar(src, pos, '{')) {
        auto errPos = getLineColFromPos(src, pos);
        std::string err = "failed to read block";

        std::string selectorStr = parent->selector;
        auto currentNode = parent->getParent();
        while (currentNode) {
            selectorStr = currentNode->selector + " > " + selectorStr;
            currentNode = currentNode->getParent();
        }

        int digitCount = numDigits(errPos.row + parent->rowOffset);

        // tried using fmt fill shit but just refused to work :(

        std::string errPart1 = fmt::format("\n Error: {}\n   --> {}/{}:{}:{}\n   --> [{}]", err, parent->packName, parent->identifier, errPos.row + parent->rowOffset, errPos.column, selectorStr);
        std::string errPart2 = fmt::format("\n {} |\n {} |     {}\n {} |     {}^", std::string(digitCount, ' '), errPos.row + parent->rowOffset, errPos.line, std::string(digitCount, ' '), std::string(errPos.offsetColumn - 1, ' '));

        log::error("{}{}", errPart1, errPart2);

        skipBlock(src, pos, 1, false);
    }
    else {
        while (pos < src.size()) {
            char c = src[pos];
            if (c == '\n' || c == '\r' || c == '}') break;
            ++pos;
        }
    }
}

bool HPTParser::matchChar(std::string_view src, size_t& pos, char c) {
    if (pos < src.size() && src[pos] == c) {
        ++pos;
        return true;
    }
    return false;
}

CCNode* HPTParser::resolveSelector(CCNode* parent, std::string_view selector) {
    if (!parent) return nullptr;
    if (selector.empty()) return parent;

    if (selector[0] == '#') {
        return parent->getChildByID(selector.substr(1));
    } else if (selector[0] == '.') {
        int index = 0;
        size_t paren = selector.find('(');
        if (paren != std::string::npos) index = numFromString<int>(selector.substr(paren+1, selector.size()-paren-2)).unwrapOr(0);
        return Utils::getChildByTypeName(parent, index, std::string(selector.substr(1, paren-1)));
    } else if (selector[0] == '?') {
        return Utils::nodeForQuery(parent, std::string(selector.substr(1)));
    } else {
        return nullptr;
    }
}

void HPTParser::applyAttribute(std::shared_ptr<HPTNode> node, std::string_view key, std::string_view value) {
    if (std::find(node->skippedAttributes.begin(), node->skippedAttributes.end(), key) != node->skippedAttributes.end() || node->skip) {
        return;
    }
    auto it = m_attributeHandlers.find(key);
    if (!node->targetNode) return;
    if (it != m_attributeHandlers.end()) it->second(node->targetNode, node, value);
}

void HPTParser::runCommand(std::shared_ptr<HPTNode> node, std::string_view key, const std::vector<std::string_view>& args) {
    auto it = m_commandHandlers.find(key);
    if (it != m_commandHandlers.end()) it->second(node->targetNode, node, args);
}

void HPTParser::setVar(std::shared_ptr<HPTNode> node, std::string_view name, std::string_view value) {
    for (HPTNode* cur = node.get(); cur; cur = cur->getParent()) {
        auto it = cur->variables.find(name);
        if (it != cur->variables.end()) {
            it->second = value;
            return;
        }
    }

    node->variables.emplace(name, value);
};

Result<std::string_view> HPTParser::getVar(std::shared_ptr<HPTNode> node, std::string_view name) {
    for (HPTNode* cur = node.get(); cur; cur = cur->getParent()) {
        auto it = cur->variables.find(name);
        if (it != cur->variables.end()) {
            auto copy = it->second;
            return Ok(process(node, copy));
        }
    }
    return Err("Variable not found");
}

std::string HPTParser::process(std::shared_ptr<HPTNode> node, std::string& value) {
    //todo allow setting the depth if needbe
    constexpr int MAX_DEPTH = 64;
    for (int depth = 0; depth < MAX_DEPTH; ++depth) {
        std::string result = processOnce(node, value);
        if (result == value) break;
        value = std::move(result);
    }
    return value;
}

std::string HPTParser::processOnce(std::shared_ptr<HPTNode> node, std::string value) {

    // do processing on value
    return value;
}

std::vector<std::string_view> HPTParser::parseArguments(std::shared_ptr<HPTNode> node, std::string_view src, size_t& pos, char startSym, char endSym, int depth) {
    std::vector<std::string_view> args;
    std::string arg;
    bool inQuote = false;
    bool quotedArg = false;
    char quoteChar = '\0';

    while (pos < src.size()) {
        char c = src[pos];

        if (inQuote) {
            if (c == '\\' && pos + 1 < src.size() && src[pos + 1] == quoteChar) {
                arg.push_back(quoteChar);
                pos += 2;
                continue;
            }
            if (c == quoteChar) {
                inQuote = false;
                ++pos;
                continue;
            }
            arg.push_back(c);
            ++pos;
            continue;
        }

        if ((c == '"' || c == '\'') && arg.empty()) {
            inQuote = true;
            quotedArg = true;
            quoteChar = c;
            ++pos;
            continue;
        }

        if (c == startSym) {
            ++depth;
            arg.push_back(c);
            ++pos;
        }
        else if (c == endSym) {
            --depth;
            ++pos;
            if (depth == 0) break;
            arg.push_back(c);
        }
        else if (c == ',' && depth == 1) {
            utils::string::trimIP(arg);
            if (!arg.empty()) args.push_back(resolveValue(node, arg));
            arg.clear();
            quotedArg = false;
            ++pos;
        }
        else {
            arg.push_back(c);
            ++pos;
        }
    }

    if (!arg.empty()) {
        utils::string::trimIP(arg);
        args.push_back(resolveValue(node, arg));
    }

    return args;
}

void HPTNode::reparse() {
    HPTParser::get().parse(data, packName, identifier, targetNode, this);
}