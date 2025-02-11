
#define HOOK_LATEST(method) queueInMainThread([&self] {\
    (void) self.setHookPriorityAfterPost(method, Utils::getHookPrioLatest(method));\
});

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

#define nodesFor(methodName) if(node) UIModding::get()->methodName(node, nodeAttributes)

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


#define setSpriteVar(varName, jsonName, type, unwrap)\
if (infoVal.contains(#jsonName)) {\
    matjson::Value val = infoVal[#jsonName];\
    if (val.is##type()) {\
        varName = val.as##type().unwrapOr(unwrap);\
    }\
}

#define setSpriteVarNum(varName, jsonName, type, unwrap)\
if (infoVal.contains(#jsonName)) {\
    matjson::Value val = infoVal[#jsonName];\
    if (val.isNumber()) {\
        varName = val.as##type().unwrapOr(unwrap);\
    }\
}

#define forEvent(type, method)\
if (eventVal.contains(#type)) {\
    matjson::Value eventType = eventVal[#type];\
    if (eventType.isObject()) {\
        eventType["_pack-name"] = nodeObject["_pack-name"];\
        button->set##method(eventType);\
    }\
}

#define setCellColors(class, method, paramType) \
struct My##class : geode::Modify<My##class, class> { \
    static void onModify(auto& self) {\
        HOOK_LATEST(#class "::" #method);\
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
		CCLayerColor* child = this->getChildByType<CCLayerColor>(0);\
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
                else if (child->getColor() == ccColor3B{230,150,10}) {\
                    std::optional<ColorData> dataOpt = UIModding::get()->getColors("list-cell-selected");\
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

#define CREATE_DUMMY(clazz) m_layers[#clazz] = createUBDummyLayer<clazz>().get(); CCNode* self = m_layers[#clazz]; \
std::map<std::string, std::pair<CCNode*, cocos2d::SEL_MenuHandler>> callbacks; \
m_callbacks[#clazz] = callbacks

#define CREATE_NORMAL(clazz) m_layers[#clazz] = clazz::create(); CCNode* self = m_layers[#clazz]; \
std::map<std::string, std::pair<CCNode*, cocos2d::SEL_MenuHandler>> callbacks; \
m_callbacks[#clazz] = callbacks

#define REGISTER_CALLBACK(clazz, method) m_callbacks[#clazz][#method] = std::pair<CCNode*, cocos2d::SEL_MenuHandler>(self, menu_selector(clazz::method))
