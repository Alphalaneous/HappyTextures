#pragma once

#include <Geode/Geode.hpp>
#include "Macros.h"

using namespace geode::prelude;

class DataNode : public CCNode {

public:

    CCNode* m_data;

    static DataNode* create(CCNode* data) {
        auto node = new DataNode;
        if (!node->init(data)) {
            CC_SAFE_DELETE(node);
            return nullptr;
        }
        node->autorelease();
        return node;
    }

    virtual bool init(CCNode* data) {
        m_data = data;
        return true;
    }
};