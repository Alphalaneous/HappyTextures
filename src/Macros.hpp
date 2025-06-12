
#define HOOK_LATEST(method)                                                     \
queueInMainThread([&self] {                                                     \
    (void) self.setHookPriority(method, INT_MIN + 1);                           \
});

#define public_cast(value, member) [](auto* v) {                                \
    class FriendClass__;                                                        \
    using T = std::remove_pointer<decltype(v)>::type;                           \
    class FriendeeClass__: public T {                                           \
    protected:                                                                  \
        friend FriendClass__;                                                   \
    };                                                                          \
    class FriendClass__ {                                                       \
    public:                                                                     \
        auto& get(FriendeeClass__* v) { return v->member; }                     \
    } c;                                                                        \
    return c.get(reinterpret_cast<FriendeeClass__*>(v));                        \
}(value)

#define typeForEaseCC(easingTypeName)                                           \
if (name == #easingTypeName) {                                                  \
    easingType = CC##easingTypeName::create(action, rate);\
}

#define typeForEaseRate(easingTypeName)                                         \
if (name == #easingTypeName) {                                                  \
    easingType = CCEase##easingTypeName::create(action, rate);                  \
}

#define typeForEase(easingTypeName)                                             \
if (name == #easingTypeName) {                                                  \
    easingType = CCEase##easingTypeName::create(action);                        \
}

#define nodesFor(methodName)                                                    \
if (node) {                                                                     \
    UIModding::get()->methodName(node, attributes);                             \
}

#define actionCase(name, ...)                                                   \
else if (type == #name) {                                                       \
    actionToDo = CC##name::create(__VA_ARGS__);                                 \
}

#define setSpriteVar(varName, jsonName, type, unwrap)                           \
if (infoVal.contains(#jsonName)) {                                              \
    matjson::Value val = infoVal[#jsonName];                                    \
    if (val.is##type()) {                                                       \
        varName = val.as##type().unwrapOr(unwrap);                              \
    }                                                                           \
}

#define setSpriteVarNum(varName, jsonName, type, unwrap)                        \
if (infoVal.contains(#jsonName)) {                                              \
    matjson::Value val = infoVal[#jsonName];                                    \
    if (val.isNumber()) {                                                       \
        varName = val.as##type().unwrapOr(unwrap);                              \
    }                                                                           \
}

#define forEvent(type, method)                                                  \
if (eventVal.contains(#type)) {                                                 \
    matjson::Value eventType = eventVal[#type];                                 \
    if (eventType.isObject()) {                                                 \
        eventType["_pack-name"] = eventVal["_pack-name"];                       \
        button->set##method(eventType);                                         \
    }                                                                           \
}

#define setCellColors(class, method, paramType)                                 \
struct My##class : geode::Modify<My##class, class> {                            \
    static void onModify(auto& self) {                                          \
        HOOK_LATEST(#class "::" #method);                                       \
    }                                                                           \
    struct Fields {                                                             \
        ccColor3B m_lastBG;                                                     \
    };                                                                          \
    void method(paramType* p0) {                                                \
        class::method(p0);                                                      \
        if (UIModding::get()->doModify) {                                       \
            checkBG(0);                                                         \
            this->schedule(schedule_selector(My##class::checkBG));              \
        }                                                                       \
    }                                                                           \
    void checkBG(float dt) {                                                    \
        auto* child = this->getChildByType<CCLayerColor>(0);                    \
        if (!child) return;                                                     \
                                                                                \
        auto color = child->getColor();                                         \
        auto fields = m_fields.self();                                          \
        if (fields->m_lastBG == color) return;                                  \
        fields->m_lastBG = color;                                               \
                                                                                \
        struct ColorMap {                                                       \
            ccColor3B match;                                                    \
            const char* ID;                                                     \
        };                                                                      \
                                                                                \
        constexpr ColorMap mappings[] = {                                       \
            {{161,  88,  44}, "list-cell-odd"},                                 \
            {{194, 114,  62}, "list-cell-even"},                                \
            {{230, 150,  10}, "list-cell-selected"},                            \
        };                                                                      \
                                                                                \
        for (const auto& entry : mappings) {                                    \
            if (color == entry.match) {                                         \
                if (auto dataOpt = UIModding::get()->getColors(entry.ID)) {     \
                    const auto& data = *dataOpt;                                \
                    child->setColor(data.color);                                \
                    child->setOpacity(data.alpha);                              \
                }                                                               \
                break;                                                          \
            }                                                                   \
        }                                                                       \
    }                                                                           \
};

#define LABEL(name, value) {name, rift::Value::from(value)}

#define STAT(key) Utils::getValidStat(#key)

#define CREATE_NORMAL(clazz)                                                    \
m_layers[#clazz] = clazz::create();                                             \
CCNode* self = m_layers[#clazz];                                                \
std::map<std::string, std::pair<CCNode*, cocos2d::SEL_MenuHandler>> callbacks;  \
m_callbacks[#clazz] = callbacks

#define REGISTER_CALLBACK(clazz, method)                                        \
m_callbacks[#clazz][#method] = std::pair<CCNode*, cocos2d::SEL_MenuHandler>     \
    (self, menu_selector(clazz::method))
