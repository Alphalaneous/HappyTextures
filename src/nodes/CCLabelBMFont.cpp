#include "CCLabelBMFont.hpp"
#include "../LabelValues.hpp"

void MyCCLabelBMFont::onModify(auto& self) {
    HOOK_LATEST("cocos2d::CCLabelBMFont::setString");
    HOOK_LATEST("cocos2d::CCLabelBMFont::limitLabelWidth");
    HOOK_LATEST("cocos2d::CCLabelBMFont::create");
}

std::string MyCCLabelBMFont::riftString(ZStringView input) {
    auto fields = m_fields.self();

    if (fields->m_isHappyTexturesModified) {
        if (!fields->m_compiled) {
            auto compiled = rift::compile(input);
            if (compiled) {
                fields->m_riftScript = std::move(compiled.unwrap());
            }
            fields->m_compiled = true;
        }
        if (fields->m_riftScript) {
            auto vars = LabelValues::getValueMap(getString());
            auto res = fields->m_riftScript->run(vars);
            if (res) {
                std::string newNewString = res.unwrap();
                return newNewString;
            }
        }
    }
    return input;
}

void MyCCLabelBMFont::setString(const char *newString, bool needUpdateLabel) {
    auto fields = m_fields.self();
    if (std::string_view(newString) != std::string_view(getString())) {
        fields->m_compiled = false;
    }
    CCLabelBMFont::setString(riftString(newString).c_str(), needUpdateLabel);
}

void MyCCLabelBMFont::setHappyTexturesModified(bool refresh) {
    m_fields->m_isHappyTexturesModified = true;
    if (refresh) {
        CCLabelBMFont::setString(getString());
    }
}

void MyCCLabelBMFont::limitLabelWidth(float width, float defaultScale, float minScale) {
    auto fields = m_fields.self();

    fields->m_limitWidth = width;
    fields->m_limitDefaultScale = defaultScale;
    fields->m_limitMinScale = minScale;
    fields->m_isLimited = true;

    CCLabelBMFont::limitLabelWidth(width, defaultScale, minScale);
}

CCLabelBMFont* MyCCLabelBMFont::create(const char *str, const char *fntFile, float width, CCTextAlignment alignment, CCPoint imageOffset) {

    auto ret = CCLabelBMFont::create(str, fntFile, width, alignment, imageOffset);

    auto myRet = static_cast<MyCCLabelBMFont*>(ret);

    bool doFix = Mod::get()->getSettingValue<bool>("pusab-fix");

    if (doFix) {
        myRet->m_fields->m_schedule = schedule_selector(MyCCLabelBMFont::checkParent);
        ret->schedule(myRet->m_fields->m_schedule);
    }
    
    return ret;
}

void MyCCLabelBMFont::checkParent(float dt) {
    if (auto parent = getParent()) {
        if (typeinfo_cast<LabelGameObject*>(parent)) {
            if (std::string_view(getFntFile()) == "bigFont.fnt") {
                ccBlendFunc blendFunc = getBlendFunc();
                setFntFile("bigFont.fnt"_spr);
                setBlendFunc(blendFunc);
            }
        }
        unschedule(m_fields->m_schedule);
    }
}