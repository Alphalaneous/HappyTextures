#pragma once

#include <Geode/Geode.hpp>
#include <rift.hpp>
#include "../Macros.hpp"
#include <alphalaneous.alphas_geode_utils/include/ObjectModify.hpp>

using namespace geode::prelude;

class $classModify(MyFLAlertLayer, FLAlertLayer) {

    struct Fields {
        TextArea* m_contentTextArea = nullptr;
        CCLabelBMFont* m_title = nullptr;
    };

    void modify();
    void setRift();
};