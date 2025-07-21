#pragma once

#include <Geode/Geode.hpp>
#include "FileWatcher.hpp"

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
    std::map<std::string, std::vector<matjson::Value>> uiCache;
    std::map<std::string, ColorData> colorCache;
    std::map<std::string, std::string> randomSprites;
    std::unordered_map<std::string, bool> filenameCache;
    std::unordered_map<CCTexture2D*, std::string> textureToNameMap;

    std::vector<FileWatcher*> listeners;
    Ref<CCArray> removalQueue = CCArray::create();
    bool doModify;
    bool skipCheck;

    void recursiveModify(CCNode* node, const matjson::Value& elements);
    void setVisible(CCNode* node, const matjson::Value& attributes);
    void setIgnoreAnchorPos(CCNode* node, const matjson::Value& attributes);
    void setScale(CCNode* node, const matjson::Value& attributes);
    void setRotation(CCNode* node, const matjson::Value& attributes);
    void setSkew(CCNode* node, const matjson::Value& attributes);
    void setAnchorPoint(CCNode* node, const matjson::Value& attributes);
    void setContentSize(CCNode* node, const matjson::Value& attributes);
    void setPosition(CCNode* node, const matjson::Value& attributes);
    void setColor(CCNode* node, const matjson::Value& attributes);
    void setText(CCNode* node, const matjson::Value& attributes);
    void setScaleMult(CCNode* node, const matjson::Value& attributes);
    void setScaleBase(CCNode* node, const matjson::Value& attributes);
    void setZOrder(CCNode* node, const matjson::Value& attributes);
    void setFont(CCNode* node, const matjson::Value& attributes);
    void setFlip(CCNode* node, const matjson::Value& attributes);
    void setBlending(CCNode* node, const matjson::Value& attributes);
    void setSpriteFrame(CCNode* node, const matjson::Value& attributes);
    void setSprite(CCNode* node, const matjson::Value& attributes);
    void setOpacity(CCNode* node, const matjson::Value& attributes);
    void setShow(CCNode* node, const matjson::Value& attributes);
    void setDisablePages(CCNode* node, const matjson::Value& attributes);
    std::string getSound(const std::string& sound);
    void playSound(CCNode* node, const matjson::Value& attributes);
    void openLink(CCNode* node, const matjson::Value& attributes);
    void setLayout(CCNode* node, const matjson::Value& attributes);
    void removeChild(CCNode* node, const matjson::Value& attributes);
    void updateLayout(CCNode* node, const matjson::Value& attributes);
    void runAction(CCNode* node, const matjson::Value& attributes);
    void runCallback(CCNode* node, const matjson::Value& attributes);
    void runScrollToTop(CCNode* node, const matjson::Value& attributes);
    void setLocked(CCNode* node, const matjson::Value& attributes);
    CCActionInterval* createAction(CCNode* node, const matjson::Value& action);
    CCActionInterval* getEasingType(const std::string& name, CCActionInterval* action, float rate);
    unsigned int stringToBlendingMode(const std::string& value);

    void handleAttributes(CCNode* node, const matjson::Value& attributes);
    void handleEvent(CCNode* node, const matjson::Value& eventVal);
    void handleParent(CCNode* node, const matjson::Value& parentVal);
    void handleRoot(CCNode* node, const matjson::Value& rootVal);
    void handleChildren(CCNode* node, matjson::Value& childrenVal);
    void handleSchedule(CCNode* node, const matjson::Value& scheduleVal);
    void handleNewChildren(CCNode* node, matjson::Value& newChildrenVal, const std::string& packName);
    void handleAllChildren(CCNode* node, const matjson::Value& allChildrenVal);
    void handleChildByIndex(CCNode* node, matjson::Value& indexChildrenVal, const std::string& packName);
    void handleModifications(CCNode* node, matjson::Value nodeObject, bool transition = false);

    void modifyChildByIndex(CCNode* node, const matjson::Value& value);

    void cleanChildren(CCNode* parentNode);
    void reloadChildren(CCNode* parentNode, bool transition = false);
    void refreshChildren(CCNode* parentNode);
    void createAndModifyNewChild(CCNode* node, const matjson::Value& newChildVal);

    void loadNodeFiles();
    void doUICheck(CCNode* node, bool afterTransition = false);
    void doUICheckForType(const std::string& type, CCNode* node);
    std::vector<std::filesystem::path> getActivePacks();
    void startFileListeners();
    AxisAlignment getAxisAlignment(const std::string& name);
    MainAxisAlignment getSimpleMainAxisAlignment(const std::string& name);
    CrossAxisAlignment getSimpleCrossAxisAlignment(const std::string& name);
    std::optional<ColorData> getColors(const std::string& name);
    void updateColors(CCNode* node, const std::string& name);
    void evaluateIf(CCNode* node, const matjson::Value& ifArray);

    static UIModding* get() {

        if (!instance) {
            instance = new UIModding();
        };
        return instance;
    }
};