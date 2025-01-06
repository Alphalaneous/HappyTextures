#pragma once

#include <Geode/Geode.hpp>
#include "FileWatcher.h"

using namespace geode::prelude;

struct ColorData {
    ccColor3B color;
    unsigned char alpha;
    bool hasColor;
};

class UIModding {

protected:
    static UIModding* instance;
public:
    std::map<std::string, matjson::Value> uiCache;
    std::map<std::string, ColorData> colorCache;
    std::map<std::string, std::string> randomSprites;
    std::vector<std::filesystem::path> activePackCache;
    std::unordered_map<std::string, bool> filenameCache;
    std::unordered_map<CCTexture2D*, std::string> textureToNameMap;
    LevelBrowserLayer* levelBrowserLayer;

    std::vector<FileWatcher*> listeners;
    Ref<CCArray> removalQueue = CCArray::create();
    bool doModify;
    bool finishedLoad;
    bool initialScene = true;

    void recursiveModify(CCNode* node, matjson::Value elements);
    void setVisible(CCNode* node, matjson::Value attributes);
    void setIgnoreAnchorPos(CCNode* node, matjson::Value attributes);
    void setScale(CCNode* node, matjson::Value attributes);
    void setRotation(CCNode* node, matjson::Value attributes);
    void setSkew(CCNode* node, matjson::Value attributes);
    void setAnchorPoint(CCNode* node, matjson::Value attributes);
    void setContentSize(CCNode* node, matjson::Value attributes);
    void setPosition(CCNode* node, matjson::Value attributes);
    void setColor(CCNode* node, matjson::Value attributes);
    void setText(CCNode* node, matjson::Value attributes);
    void setScaleMult(CCNode* node, matjson::Value attributes);
    void setScaleBase(CCNode* node, matjson::Value attributes);
    void setZOrder(CCNode* node, matjson::Value attributes);
    void setFont(CCNode* node, matjson::Value attributes);
    void setFlip(CCNode* node, matjson::Value attributes);
    void setBlending(CCNode* node, matjson::Value attributes);
    void setSpriteFrame(CCNode* node, matjson::Value attributes);
    void setSprite(CCNode* node, matjson::Value attributes);
    void setOpacity(CCNode* node, matjson::Value attributes);
    void setShow(CCNode* node, matjson::Value attributes);
    void setDisablePages(CCNode* node, matjson::Value attributes);
    std::string getSound(std::string sound);
    void playSound(CCNode* node, matjson::Value attributes);
    void openLink(CCNode* node, matjson::Value attributes);
    void setLayout(CCNode* node, matjson::Value attributes);
    void removeChild(CCNode* node, matjson::Value attributes);
    void updateLayout(CCNode* node, matjson::Value attributes);
    void runAction(CCNode* node, matjson::Value attributes);
    void runCallback(CCNode* node, matjson::Value attributes);
    void runScrollToTop(CCNode* node, matjson::Value attributes);
    void setLocked(CCNode* node, matjson::Value attributes);
    CCActionInterval* createAction(CCNode* node, matjson::Value action);
    CCActionInterval* getEasingType(std::string name, CCActionInterval* action, float rate);
    unsigned int stringToBlendingMode(std::string value);
    void handleModifications(CCNode* node, matjson::Value nodeObject);
    void loadNodeFiles();
    void doUICheck(CCNode* node, bool afterTransition = false);
    void doUICheckForType(std::string name, CCNode* node);
    std::vector<std::filesystem::path> getActivePacks();
    void startFileListeners();
    AxisAlignment getAxisAlignment(std::string name);
    std::optional<ColorData> getColors(std::string name);
    void updateColors(CCNode* node, std::string name);
    void evaluateIf(CCNode* node, matjson::Value ifArray);

    static UIModding* get() {

        if (!instance) {
            instance = new UIModding();
        };
        return instance;
    }
};