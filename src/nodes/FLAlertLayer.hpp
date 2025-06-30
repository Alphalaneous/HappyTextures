#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/FLAlertLayer.hpp>
#include <rift.hpp>
#include "CCLabelBMFont.hpp"
#include "TextArea.hpp"
#include "../Macros.hpp"

class $modify(MyFLAlertLayer, FLAlertLayer) {

    static void onModify(auto& self) {
        HOOK_LATEST("FLAlertLayer::init");
    }

    struct Fields {
        TextArea* m_contentTextArea = nullptr;
        CCLabelBMFont* m_title = nullptr;
    };

    bool init(FLAlertLayerProtocol* p0, char const* p1, gd::string p2, char const* p3, char const* p4, float p5, bool p6, float p7, float p8) {
        if (!FLAlertLayer::init(p0, p1, p2, p3, p4, p5, p6, p7, p8)) return false;

        if (m_mainLayer) {
            if (CCNode* textArea = m_mainLayer->getChildByIDRecursive("content-text-area")) {
                m_fields->m_contentTextArea = static_cast<TextArea*>(textArea);
            }
            if (CCNode* title = m_mainLayer->getChildByID("title")) {
                m_fields->m_title = static_cast<CCLabelBMFont*>(title);
            }
        }
        return true;
    }

    void setRift() {

        if (m_fields->m_contentTextArea) {
            MyTextArea* myTextArea = static_cast<MyTextArea*>(m_fields->m_contentTextArea);
            myTextArea->setHappyTexturesModified(true);
        }

        if (m_fields->m_title) {
            MyCCLabelBMFont* myTitle = static_cast<MyCCLabelBMFont*>(m_fields->m_title);
            myTitle->setHappyTexturesModified(true);
        }
    }
};