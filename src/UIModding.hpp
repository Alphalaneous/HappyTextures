#pragma once

#include <Geode/Geode.hpp>
#include <unordered_map>
#include "FileWatcher.hpp"

using namespace geode::prelude;

struct ColorData {
    ccColor3B color;
    unsigned char alpha;
    bool hasColor;
};

struct UIData {
    std::string packName;
    std::string data;
};

class UIModding {

protected:
    static UIModding* instance;
public:
    StringMap<std::vector<matjson::Value>> uiCache;
    StringMap<std::vector<UIData>> uiCacheHpt;
    StringMap<ColorData> colorCache;
    StringMap<std::string> randomSprites;
    StringMap<bool> filenameCache;
    std::unordered_map<CCTexture2D*, std::string> textureToNameMap;
    bool firstMenuLayer = true;

    std::unordered_map<CCObject*, uintptr_t> m_nodeVTables;
    std::unordered_map<const std::type_info*, bool> m_isNodeCache;

    std::vector<FileWatcher*> listeners;
    std::vector<Ref<CCNode>> removalQueue;

    std::unordered_map<Ref<CCNode>, std::function<void()>> moveQueue;
    bool doModify;
    bool skipCheck;

    void queryModify(CCNode* node, const matjson::Value& elements);
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
    void setSprite(CCNode* node, const matjson::Value& attributes);
    void setOpacity(CCNode* node, const matjson::Value& attributes);
    void setShow(CCNode* node, const matjson::Value& attributes);
    void setDisablePages(CCNode* node, const matjson::Value& attributes);
    std::string getSound(std::string_view sound);
    void playSound(CCNode* node, const matjson::Value& attributes);
    void openLink(CCNode* node, const matjson::Value& attributes);
    void setLayout(CCNode* node, const matjson::Value& attributes);
    void removeChild(CCNode* node, const matjson::Value& attributes);
    void updateLayout(CCNode* node, const matjson::Value& attributes);
    void runAction(CCNode* node, const matjson::Value& attributes);
    void runCallback(CCNode* node, const matjson::Value& attributes);
    void runScrollToTop(CCNode* node, const matjson::Value& attributes);
    void setLocked(CCNode* node, const matjson::Value& attributes);

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
    void handleMoveChild(CCNode* node, const matjson::Value& attributes, const std::string& packName);

    void modifyChildByIndex(CCNode* node, const matjson::Value& value);

    void cleanChildren(CCNode* parentNode);
    void reloadChildren(CCNode* parentNode, bool transition = false);
    void refreshChildren(CCNode* parentNode);
    void createAndModifyNewChild(CCNode* node, const matjson::Value& newChildVal);

    void loadNodeFiles();
    void doUICheck(CCNode* node, bool afterTransition = false);
    void doUICheckForType(std::string_view type, CCNode* node);
    std::vector<std::filesystem::path> getActivePacks();
    void startFileListeners();
    AxisAlignment getAxisAlignment(const std::string& name);
    MainAxisAlignment getSimpleMainAxisAlignment(const std::string& name);
    CrossAxisAlignment getSimpleCrossAxisAlignment(const std::string& name);
    std::optional<ColorData> getColors(const std::string& name);
    void updateColors(CCNode* node, const std::string& name);
    void evaluateIf(CCNode* node, const matjson::Value& ifArray);
    std::vector<std::string> generateValidSprites(const std::string& path, const matjson::Value& spriteList);

    bool checkNodeValidity(CCObject* node);
    void setNodeVTable(CCNode* node);
    bool isNode(CCObject* obj);

    static UIModding* get();
};