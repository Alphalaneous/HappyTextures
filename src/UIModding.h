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
void setFlip(CCNode* node, matjson::Object attributes);
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
void doUICheckForType(std::string name, CCNode* node);

std::vector<std::string> getActivePacks();
void startFileListeners();
AxisAlignment getAxisAlignment(std::string name);

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

#define handleModifyForType(typeName) \
if(type == #typeName){\
    handleModifications(getChildOfType<typeName>(node, index), childObject);\
}

#define nodesFor(methodName) if(node) methodName(node, nodeAttributesObject)

#define actionForName2(name, x, y, param2) if(type == #name){ \
            if(!isNumber){ \
                actionToDo = CC##name::create(duration, x, y); \
            } \
            else { \
                actionToDo = CC##name::create(duration, param2); \
            } \
        }

#define actionForName(name, params) if(type == #name){ \
            if(!isNumber){ \
                actionToDo = CC##name::create##params; \
            } \
        }

#define modifyNodeForType(name, method, params, values) \
class $modify(name){    \
\
    static void onModify(auto& self) {\
        (void) self.setHookPriority(#name "::" #method, INT_MIN);\
    }\
\
    bool method##params { \
        if(!name::method##values)  return false;\
        if(doModify){\
            doUICheckForType(#name, this);\
        }\
        return true;\
    }\
};

#define modifyNodeForTypeCreate(name, method, params, values) \
class $modify(name){    \
\
    static void onModify(auto& self) {\
        (void) self.setHookPriority(#name "::" #method, INT_MIN);\
    }\
\
    name* method##params { \
        auto ret = name::method##values; \
        if(doModify){\
            doUICheckForType(#name, ret);\
        }\
        return ret;\
    }\
};

#define setSpriteVar(varName, jsonName, type)\
if(infoVal.contains(#jsonName)){\
    matjson::Value val = infoVal[#jsonName];\
    if(val.is_##type()){\
        varName = val.as_##type();\
    }\
}

#define setSpriteVarNum(varName, jsonName, type)\
if(infoVal.contains(#jsonName)){\
    matjson::Value val = infoVal[#jsonName];\
    if(val.is_number()){\
        varName = val.as_##type();\
    }\
}

#endif