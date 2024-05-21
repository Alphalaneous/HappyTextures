#define public_cast(value, member) [](auto* v) { \
    class FriendClass__; \
    using T = std::remove_pointer<decltype(v)>::type; \
    class FriendeeClass__: public T { \
    protected: \
        friend FriendClass__; \
    }; \
    class FriendClass__ { \
    public: \
        auto& get(FriendeeClass__* v) { return v->member; } \
    } c; \
    return c.get(reinterpret_cast<FriendeeClass__*>(v)); \
}(value)

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
    UIModding::get()->handleModifications(getChildOfType<typeName>(node, index), childObject);\
}

#define nodesFor(methodName) if(node) UIModding::get()->methodName(node, nodeAttributesObject)

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
        if(UIModding::get()->doModify){\
            UIModding::get()->doUICheckForType(#name, this);\
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
        if(UIModding::get()->doModify){\
            UIModding::get()->doUICheckForType(#name, ret);\
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

#define forEvent(type, method)\
if(eventObject.contains(#type)){\
    matjson::Value eventVal = eventObject[#type];\
    if(eventVal.is_object()){\
        matjson::Object event = eventVal.as_object();\
        event["_pack-name"] = nodeObject["_pack-name"];\
        button->set##method(event);\
    }\
}
