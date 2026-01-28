#include "FLAlertLayer.hpp"
#include "CCLabelBMFont.hpp"
#include "TextArea.hpp"

void MyFLAlertLayer::modify() {
    auto fields = m_fields.self();
    if (m_mainLayer) {
        if (CCNode* textArea = m_mainLayer->getChildByIDRecursive("content-text-area")) {
            fields->m_contentTextArea = static_cast<TextArea*>(textArea);
        }
        if (CCNode* title = m_mainLayer->getChildByID("title")) {
            fields->m_title = static_cast<CCLabelBMFont*>(title);
        }
    }
}

void MyFLAlertLayer::setRift() {
    auto fields = m_fields.self();
    if (fields->m_contentTextArea) {
        MyTextArea* myTextArea = static_cast<MyTextArea*>(fields->m_contentTextArea);
        myTextArea->setHappyTexturesModified(true);
    }

    if (fields->m_title) {
        MyCCLabelBMFont* myTitle = static_cast<MyCCLabelBMFont*>(fields->m_title);
        myTitle->setHappyTexturesModified(true);
    }
}