#pragma once
#include <Geode/Geode.hpp>
#include "Geode/utils/StringMap.hpp"
#include "HPTParser.hpp"

using namespace geode::prelude;

using NodeHandler = std::function<CCNode*(std::shared_ptr<HPTNode>)>;

class NodeFactory {
public:
    static NodeFactory& get();
    void setupNodes();
    void registerNode(std::string_view type, NodeHandler nodeHandler);
    NodeHandler getNodeHandler(std::string_view type);
private:
    StringMap<NodeHandler> m_nodeHandlers;
};