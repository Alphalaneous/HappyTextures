#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCTextInputNode.hpp>
#include "../Macros.h"

class $modify(MyCCTextInputNode, CCTextInputNode) {

    /*void refreshLabel() {
        CCTextInputNode::refreshLabel();
        if (m_placeholderLabel && m_placeholderLabel->getColor() == ccColor3B{120, 170, 240}) {
            m_placeholderLabel->setColor({0, 0, 0});

            std::optional<ColorData> dataOpt = UIModding::get()->getColors("text-input-placeholder");
            if(dataOpt.has_value()){
                ColorData data = dataOpt.value();
                //m_placeholderLabel->setColor(data.color);
                m_placeholderLabel->setOpacity(data.alpha);
            }
        }
    }*/ //todo find method I can hook to change this
};