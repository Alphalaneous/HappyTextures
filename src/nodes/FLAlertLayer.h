#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/FLAlertLayer.hpp>
#include <rift.hpp>
#include "CCLabelBMFont.h"
#include "TextArea.h"

class $modify(MyFLAlertLayer, FLAlertLayer) {

    struct Fields {
        TextArea* m_contentTextArea;
        CCLabelBMFont* m_title;
    };

    bool init(FLAlertLayerProtocol* p0, char const* p1, gd::string p2, char const* p3, char const* p4, float p5, bool p6, float p7, float p8) {
        if (!FLAlertLayer::init(p0, p1, p2, p3, p4, p5, p6, p7, p8)) return false;

        m_fields->m_contentTextArea = static_cast<TextArea*>(m_mainLayer->getChildByIDRecursive("content-text-area"));
        m_fields->m_title = static_cast<CCLabelBMFont*>(m_mainLayer->getChildByID("title"));

        return true;
    }

    void setRift() {

        MyTextArea* myTextArea = static_cast<MyTextArea*>(m_fields->m_contentTextArea);
        myTextArea->setHappyTexturesModified(true);

        MyCCLabelBMFont* myTitle = static_cast<MyCCLabelBMFont*>(m_fields->m_title);
        myTitle->setHappyTexturesModified(true);
    }
};