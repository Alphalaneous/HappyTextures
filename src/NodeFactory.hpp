#pragma once
#include <Geode/Geode.hpp>
#include "HPTParser.hpp"
#include "StringUtils.hpp"

using namespace geode::prelude;

using NodeHandler = std::function<CCNode*(std::shared_ptr<HPTNode>)>;

class NodeFactory {
public:
    static NodeFactory& get();
    void setupNodes();
    void registerNode(std::string_view type, NodeHandler nodeHandler);
    NodeHandler getNodeHandler(std::string_view type);
private:
    std::unordered_map<std::string, NodeHandler, StringHash, StringEq> m_nodeHandlers;
};