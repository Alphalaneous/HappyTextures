#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCLabelBMFont.hpp>
#include "../Utils.h"
#include "../LabelValues.h"
#include "../Macros.h"

#include <rift.hpp>

using namespace geode::prelude;

class $modify(MyCCLabelBMFont, CCLabelBMFont) {

    static void onModify(auto& self) {
        HOOK_LATEST("cocos2d::CCLabelBMFont::setString");
        HOOK_LATEST("cocos2d::CCLabelBMFont::limitLabelWidth");
        HOOK_LATEST("cocos2d::CCLabelBMFont::create");
    }

    struct Fields {
        float m_limitWidth = 1;
        float m_limitDefaultScale = 1;
        float m_limitMinScale = 1;
        bool m_isLimited = false;
        SEL_SCHEDULE m_schedule;
        bool m_isHappyTexturesModified;
        std::unique_ptr<rift::Script> m_riftScript;
        bool m_compiled = false;
    };
    
    std::string riftString(std::string input) {
        auto fields = m_fields.self();

        if (fields->m_isHappyTexturesModified) {
            if (!fields->m_compiled) {
                auto compiled = rift::compile(std::string_view(input));
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

    void setString(const char *newString, bool needUpdateLabel) {
        auto fields = m_fields.self();
        if (std::string_view(newString) != std::string_view(getString())) {
            fields->m_compiled = false;
        }
        CCLabelBMFont::setString(riftString(newString).c_str(), needUpdateLabel);
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

        bool doFix = Mod::get()->getSettingValue<bool>("pusab-fix");

        if (doFix) {
            myRet->m_fields->m_schedule = schedule_selector(MyCCLabelBMFont::checkParent);
            ret->schedule(myRet->m_fields->m_schedule);
        }
        
        return ret;
    }

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
};