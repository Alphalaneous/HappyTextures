#pragma once

#include <Geode/Geode.hpp>
#include "Macros.hpp"

using namespace geode::prelude;

class DataNode : public CCNode {

public:

    CCNode* m_data;

    static DataNode* create(CCNode* data) {
        auto node = new DataNode();
        if (node->init(data)) {
            node->autorelease();
            return node;
        }
        delete node;
        return nullptr;
    }

    virtual bool init(CCNode* data) {
        m_data = data;
        return true;
    }
};