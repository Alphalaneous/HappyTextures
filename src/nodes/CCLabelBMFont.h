#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCLabelBMFont.hpp>
#include "../Utils.h"
#include "../LabelValues.h"
#include <rift.hpp>


using namespace geode::prelude;

class $modify(MyCCLabelBMFont, CCLabelBMFont) {

    struct Fields {
        float m_limitWidth = 1;
        float m_limitDefaultScale = 1;
        float m_limitMinScale = 1;
        bool m_isLimited = false;
        SEL_SCHEDULE m_schedule;
        bool m_isHappyTexturesModified;
    };
    
    static std::string riftString(MyCCLabelBMFont* self, std::string input) {
        if (self->m_fields->m_isHappyTexturesModified) {
            rift::Result<rift::Script*> scriptRes = rift::compile(std::string_view(input));
            if (!scriptRes.isError()) {
                rift::Script* script = scriptRes.unwrap();
                auto vars = LabelValues::getValueMap(self->getString());
                std::string newNewString = script->run(vars);
                delete script;
                return newNewString;
            }
        }
        return input;
    }

    void setString(const char *newString, bool needUpdateLabel) {
        CCLabelBMFont::setString(riftString(this, newString).c_str(), needUpdateLabel);
    }

    void setHappyTexturesModified(bool refresh = false) {
        m_fields->m_isHappyTexturesModified = true;
        if (refresh) {
            CCLabelBMFont::setString(getString());
        }
    }

    void limitLabelWidth(float width, float defaultScale, float minScale) {

        m_fields->m_limitWidth = width;
        m_fields->m_limitDefaultScale = defaultScale;
        m_fields->m_limitMinScale = minScale;
        m_fields->m_isLimited = true;

        CCLabelBMFont::limitLabelWidth(width, defaultScale, minScale);
    }

    static CCLabelBMFont* create(const char *str, const char *fntFile, float width, CCTextAlignment alignment, CCPoint imageOffset) {

        auto ret = CCLabelBMFont::create(str, fntFile, width, alignment, imageOffset);

        auto myRet = static_cast<MyCCLabelBMFont*>(ret);

        #ifndef GEODE_IS_MACOS

        bool doFix = Mod::get()->getSettingValue<bool>("pusab-fix");

        if (doFix) {
            myRet->m_fields->m_schedule = schedule_selector(MyCCLabelBMFont::checkParent);
            ret->schedule(myRet->m_fields->m_schedule);
        }

        #endif
        
        return ret;
    }

    #ifndef GEODE_IS_MACOS

    void checkParent(float dt) {
        
        if (auto parent = this->getParent()) {
            if (typeinfo_cast<LabelGameObject*>(parent)) {
                if (strcmp(this->getFntFile(), "bigFont.fnt") == 0) {
                    ccBlendFunc blendFunc = this->getBlendFunc();
                    this->setFntFile("bigFont.fnt"_spr);
                    this->setBlendFunc(blendFunc);
                }
            }
            this->unschedule(m_fields->m_schedule);
        }
    }

    #endif
};