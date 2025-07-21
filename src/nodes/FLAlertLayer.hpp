#pragma once

#include <Geode/Geode.hpp>
#include <rift.hpp>
#include "CCLabelBMFont.hpp"
#include "TextArea.hpp"
#include "../Macros.hpp"
#include <alphalaneous.alphas_geode_utils/include/NodeModding.h>

class $nodeModify(MyFLAlertLayer, FLAlertLayer) {

    struct Fields {
        TextArea* m_contentTextArea = nullptr;
        CCLabelBMFont* m_title = nullptr;
    };

    void modify() {
        auto self = reinterpret_cast<FLAlertLayer*>(this);
        if (self->m_mainLayer) {
            if (CCNode* textArea = self->m_mainLayer->getChildByIDRecursive("content-text-area")) {
                m_fields->m_contentTextArea = static_cast<TextArea*>(textArea);
            }
            if (CCNode* title = self->m_mainLayer->getChildByID("title")) {
                m_fields->m_title = static_cast<CCLabelBMFont*>(title);
            }
        }
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