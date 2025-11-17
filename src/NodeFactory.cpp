#include "NodeFactory.hpp"

NodeFactory& NodeFactory::get() {
    static NodeFactory instance;
    return instance;
}

void NodeFactory::setupNodes() {
    registerNode("CCNode", [] (std::shared_ptr<HPTNode> hptNode) {
        hptNode->skippedAttributes.push_back("scale");
        return CCNode::create();
    });
}

void NodeFactory::registerNode(std::string_view type, NodeHandler nodeHandler) {
    m_nodeHandlers.emplace(type, nodeHandler);
}

NodeHandler NodeFactory::getNodeHandler(std::string_view type) {
    auto it = m_nodeHandlers.find(type);
    return it == m_nodeHandlers.end() ? nullptr : it->second;
}