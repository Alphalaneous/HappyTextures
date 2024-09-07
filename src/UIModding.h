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
    std::vector<FileWatcher*> listeners;
    Ref<CCArray> removalQueue = CCArray::create();
    bool doModify;

    void recursiveModify(CCNode* node, matjson::Object elements);
    void setVisible(CCNode* node, matjson::Object attributes);
    void setIgnoreAnchorPos(CCNode* node, matjson::Object attributes);
    void setScale(CCNode* node, matjson::Object attributes);
    void setRotation(CCNode* node, matjson::Object attributes);
    void setSkew(CCNode* node, matjson::Object attributes);
    void setAnchorPoint(CCNode* node, matjson::Object attributes);
    void setContentSize(CCNode* node, matjson::Object attributes);
    void setPosition(CCNode* node, matjson::Object attributes);
    void setColor(CCNode* node, matjson::Object attributes);
    void setText(CCNode* node, matjson::Object attributes);
    void setScaleMult(CCNode* node, matjson::Object attributes);
    void setScaleBase(CCNode* node, matjson::Object attributes);
    void setZOrder(CCNode* node, matjson::Object attributes);
    void setFont(CCNode* node, matjson::Object attributes);
    void setFlip(CCNode* node, matjson::Object attributes);
    void setBlending(CCNode* node, matjson::Object attributes);
    void setSpriteFrame(CCNode* node, matjson::Object attributes);
    void setSprite(CCNode* node, matjson::Object attributes);
    void setOpacity(CCNode* node, matjson::Object attributes);
    void setShow(CCNode* node, matjson::Object attributes);
    void setDisablePages(CCNode* node, matjson::Object attributes);
    std::string getSound(std::string sound);
    void playSound(CCNode* node, matjson::Object attributes);
    void openLink(CCNode* node, matjson::Object attributes);
    void setLayout(CCNode* node, matjson::Object attributes);
    void removeChild(CCNode* node, matjson::Object attributes);
    void updateLayout(CCNode* node, matjson::Object attributes);
    void runAction(CCNode* node, matjson::Object attributes);
    CCActionInterval* createAction(CCNode* node, matjson::Value action);
    CCActionInterval* getEasingType(std::string name, CCActionInterval* action, float rate);
    unsigned int stringToBlendingMode(std::string value);
    void handleModifications(CCNode* node, matjson::Object nodeObject);
    void doUICheck(CCNode* node);
    void doUICheckForType(std::string name, CCNode* node);
    std::vector<std::string> getActivePacks();
    void startFileListeners();
    AxisAlignment getAxisAlignment(std::string name);
    std::optional<ColorData> getColors(std::string name);
    void updateColors(CCNode* node, std::string name);

    static UIModding* get(){

        if (!instance) {
            instance = new UIModding();
        };
        return instance;
    }
};