#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCSpriteBatchNode.hpp>
#include <Geode/modify/CCSprite.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include "../Config.h"

using namespace geode::prelude;

class BatchHandler {
//this exists to efficiently add fields without to much of a performance impact
public:
    std::unordered_map<cocos2d::CCSpriteBatchNode*, bool> m_batchNodes;
    static BatchHandler& get() {
        static BatchHandler instance;
        return instance;
    }
    bool isFake(cocos2d::CCSpriteBatchNode* node) {
		if (typeinfo_cast<CCLabelBMFont*>(node) || node->getUserObject("dont-fake"_spr)) return false;
		return !Config::get()->m_batchNodesEnabled;
		//if disabling individual batch nodes gets added, uncomment this out
        //return m_batchNodes[node];
    }
};

class $modify(GJBaseGameLayer) {

    void setupLayers() {
		GJBaseGameLayer::setupLayers();
		for (CCSpriteBatchNode* node : CCArrayExt<CCSpriteBatchNode*>(m_batchNodes)) {
			node->setUserObject("dont-fake"_spr, CCBool::create(true));
		}
	}

};

#define DO_FAKE(method) if (!BatchHandler::get().isFake(this)) CCSpriteBatchNode::method; else CCNode::method;

class $modify(MyCCSpriteBatchNode, CCSpriteBatchNode) {
	struct Fields {
		Ref<CCSpriteBatchNode> m_self;
		~Fields() {
            BatchHandler::get().m_batchNodes.erase(m_self);
        }
	};
	static CCSpriteBatchNode* createWithTexture(CCTexture2D* tex, unsigned int capacity) {
        auto ret = CCSpriteBatchNode::createWithTexture(tex, capacity);
		static_cast<MyCCSpriteBatchNode*>(ret)->m_fields->m_self = ret;
		BatchHandler::get().m_batchNodes[ret] = false;
		return ret;
    }
	void setFake(bool fake) {
		BatchHandler::get().m_batchNodes[this] = true;
	}
	bool isFake() {
		return BatchHandler::get().isFake(this);
	}
	void draw() {
		DO_FAKE(draw());
	}
    void reorderChild(CCNode* child, int zOrder) {
		DO_FAKE(reorderChild(child, zOrder));
	}
    void removeChild(CCNode* child, bool cleanup) {
		DO_FAKE(removeChild(child, cleanup));
	}
    void removeAllChildrenWithCleanup(bool cleanup) {
		DO_FAKE(removeAllChildrenWithCleanup(cleanup));
	}
	void sortAllChildren() {
		DO_FAKE(sortAllChildren());
	}
	void visit() {
		DO_FAKE(visit());
	}
	void recursiveBlend(CCNode* node, ccBlendFunc blendFunc) {
		if (node) {
			if (CCBlendProtocol* blendNode = typeinfo_cast<CCBlendProtocol*>(node)) {
				blendNode->setBlendFunc(blendFunc);
			}
			for (CCNode* child : CCArrayExt<CCNode*>(node->getChildren())){
				if (CCBlendProtocol* blendNode = typeinfo_cast<CCBlendProtocol*>(child)) {
					recursiveBlend(child, blendFunc);
				}
			}
		}
	}
	void addChild(CCNode* child, int zOrder, int tag) {
		if (BatchHandler::get().isFake(this)) {
			if (CCBlendProtocol* blendNode = typeinfo_cast<CCBlendProtocol*>(child)) {
				recursiveBlend(child, getBlendFunc());
			}
		}
		DO_FAKE(addChild(child, zOrder, tag));
	}
    void setBlendFunc(ccBlendFunc blendFunc) {
		CCSpriteBatchNode::setBlendFunc(blendFunc);
		if (BatchHandler::get().isFake(this)) {
			for (CCNode* child : CCArrayExt<CCNode*>(getChildren())){
				if (CCBlendProtocol* blendNode = typeinfo_cast<CCBlendProtocol*>(child)) {
					recursiveBlend(child, blendFunc);
				}
			}
		}
	}
};

class $modify(BatchlessCCSprite, CCSprite) {

	struct Fields {
		MyCCSpriteBatchNode* m_originalBatchNode;
	};

	void setBatchNode(CCSpriteBatchNode *pobSpriteBatchNode) {
		MyCCSpriteBatchNode* spn = nullptr;
		if (pobSpriteBatchNode) {
			spn = static_cast<MyCCSpriteBatchNode*>(pobSpriteBatchNode);
			m_fields->m_originalBatchNode = spn;
			if (spn->isFake()) {
				CCSprite::setBatchNode(nullptr);
			}
		}
		
		if (spn && spn->isFake()) {
			setBlendFunc(spn->getBlendFunc());
			return;
		}
		CCSprite::setBatchNode(pobSpriteBatchNode);
	}
};