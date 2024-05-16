#pragma once

#ifndef UIMODDING_H
#define UIMODDING_H

#include <Geode/Geode.hpp>

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
void setZOrder(CCNode* node, matjson::Object attributes);
void setFont(CCNode* node, matjson::Object attributes);
void setBlending(CCNode* node, matjson::Object attributes);
void playSound(CCNode* node, matjson::Object attributes);
void openLink(CCNode* node, matjson::Object attributes);
void setLayout(CCNode* node, matjson::Object attributes);
void updateLayout(CCNode* node, matjson::Object attributes);
void runAction(CCNode* node, matjson::Object attributes);
CCActionInterval* createAction(matjson::Value action);
CCActionInterval* getEasingType(std::string name, CCActionInterval* action, float rate);
unsigned int stringToBlendingMode(std::string value);
void handleModifications(CCNode* node, matjson::Object nodeObject);
void doUICheck(CCNode* node);
std::vector<std::string> getActivePacks();
void startFileListeners();

#define typeForEaseCC(easingTypeName) \
if(name == #easingTypeName){\
	easingType = CC##easingTypeName::create(action, rate);\
}

#define typeForEaseRate(easingTypeName) \
if(name == #easingTypeName){\
	easingType = CCEase##easingTypeName::create(action, rate);\
}

#define typeForEase(easingTypeName) \
if(name == #easingTypeName){\
	easingType = CCEase##easingTypeName::create(action);\
}

#define modifyForType(typeName) \
if(type == #typeName){\
    handleModifications(getChildOfType<typeName>(node, index), childObject);\
}

#define nodesFor(methodName) set##methodName(node, nodeAttributesObject)

#define nodesForMethod(methodName) methodName(node, nodeAttributesObject)

#endif