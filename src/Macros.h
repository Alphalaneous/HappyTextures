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
if (name == #easingTypeName) {\
    easingType = CC##easingTypeName::create(action, rate);\
}

#define typeForEaseRate(easingTypeName) \
if (name == #easingTypeName) {\
    easingType = CCEase##easingTypeName::create(action, rate);\
}

#define typeForEase(easingTypeName) \
if (name == #easingTypeName) {\
    easingType = CCEase##easingTypeName::create(action);\
}

#define handleModifyForType(typeName) \
if (type == #typeName) {\
    UIModding::get()->handleModifications(getChildOfType<typeName>(node, index), childObject);\
}

#define nodesFor(methodName) if(node) UIModding::get()->methodName(node, nodeAttributesObject)

#define actionForName2(name, x, y, param2) if(type == #name){ \
            if (!isNumber) { \
                actionToDo = CC##name::create(duration, x, y); \
            } \
            else { \
                actionToDo = CC##name::create(duration, param2); \
            } \
        }

#define actionForName(name, params) if(type == #name){ \
            if (!isNumber) { \
                actionToDo = CC##name::create params; \
            } \
        }

#define modifyNodeForType(name, method, params, values) \
class $modify(name){    \
\
    static void onModify(auto& self) {\
        (void) self.setHookPriority(#name "::" #method, INT_MIN);\
    }\
\
    bool method params { \
        if (!name::method values) return false;\
        if (UIModding::get()->doModify) {\
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
    static name* method params { \
        auto ret = name::method values; \
        if (UIModding::get()->doModify) {\
            UIModding::get()->doUICheckForType(#name, ret);\
        }\
        return ret;\
    }\
};

#define setSpriteVar(varName, jsonName, type)\
if (infoVal.contains(#jsonName)) {\
    matjson::Value val = infoVal[#jsonName];\
    if (val.is_##type()) {\
        varName = val.as_##type();\
    }\
}

#define setSpriteVarNum(varName, jsonName, type)\
if (infoVal.contains(#jsonName)) {\
    matjson::Value val = infoVal[#jsonName];\
    if (val.is_number()) {\
        varName = val.as_##type();\
    }\
}

#define forEvent(type, method)\
if (eventObject.contains(#type)) {\
    matjson::Value eventVal = eventObject[#type];\
    if (eventVal.is_object()) {\
        matjson::Object event = eventVal.as_object();\
        event["_pack-name"] = nodeObject["_pack-name"];\
        button->set##method(event);\
    }\
}

#define setCellColors(class, method, paramType) \
struct My##class : geode::Modify<My##class, class> { \
    static void onModify(auto& self) {\
        (void) self.setHookPriority(#class "::" #method, INT_MIN);\
    }\
	struct Fields {\
		ccColor3B m_lastBG;\
	};\
	void method(paramType* p0){\
		class::method(p0);\
        if (UIModding::get()->doModify) {\
            checkBG(0);\
            this->schedule(schedule_selector(My##class::checkBG));\
        }\
	}\
	void checkBG(float dt) {\
		CCLayerColor* child = getChildOfType<CCLayerColor>(this, 0);\
		if (child) {\
			if (m_fields->m_lastBG != child->getColor()) {\
				m_fields->m_lastBG = child->getColor();\
                if (child->getColor() == ccColor3B{161,88,44}) {\
                    std::optional<ColorData> dataOpt = UIModding::get()->getColors("list-cell-odd");\
                    if (dataOpt.has_value()) {\
                        ColorData data = dataOpt.value();\
                        child->setColor(data.color);\
                        child->setOpacity(data.alpha);\
                    }\
                }\
                else if (child->getColor() == ccColor3B{194,114,62}) {\
                    std::optional<ColorData> dataOpt = UIModding::get()->getColors("list-cell-even");\
                    if (dataOpt.has_value()) {\
                        ColorData data = dataOpt.value();\
                        child->setColor(data.color);\
                        child->setOpacity(data.alpha);\
                    }\
                }\
			}\
		}\
	}\
};

#define SAFE_RUN(method) retain(); method release();

#define LABEL(name, value) {name, rift::Value::from(value)}

#define STAT(key) Utils::getValidStat(#key)