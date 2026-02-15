#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

struct HPTNode;

using AttributeHandler = std::function<void(CCNode*, std::shared_ptr<HPTNode>, std::string_view)>;
using AttributeExportHandler = std::function<std::string_view(CCNode*)>;

using CommandHandler = std::function<void(CCNode*, std::shared_ptr<HPTNode>, const std::vector<std::string_view>&)>;
using MethodHandler = std::function<std::string_view(const std::vector<std::string_view>&)>;

struct ErrorInfo {
    int row;
    int column;
    int offsetColumn;
    std::string line;
};

struct HPTNodeTypeDef {
    std::string name;
    bool isAttribute;
    bool skipParsing;
};

struct HPTNode {
    std::string selector;
    std::string packName;
    std::string identifier;
    std::string data;
    int rowOffset;
    bool skip;
    StringMap<std::string> attributes;
    std::vector<std::string> skippedAttributes;
    std::vector<std::shared_ptr<HPTNode>> children;
    StringMap<std::string> variables;

    HPTNode* parent;
    CCNode* targetNode = nullptr;
    HPTNodeTypeDef typeDef = {"Default", false};

    virtual Result<HPTNode> operator[](const std::string& selector) {
        return get(selector);
    }

    HPTNode* getParent() {
        return parent;
    }

    Result<HPTNode> get(const std::string& selector) {
        for (auto child : children) {
            if (child->selector == selector) return Ok(*child);
        }
        return Err("No node found");
    }

    void reparse();
};

class HPTParser {
public:
    static HPTParser& get();

    std::shared_ptr<HPTNode> parse(std::string_view src, std::string_view packName, std::string_view identifier, CCNode* rootNode, HPTNode* originalNode = nullptr);

    void registerAttribute(std::string_view name, AttributeHandler handler, AttributeExportHandler exportHandler, bool skipParsing = false);
    void registerCommand(std::string_view name, CommandHandler handler);
    void registerMethod(std::string_view name, MethodHandler handler);
    void registerNodeType(std::string_view name, const HPTNodeTypeDef& def);
    void setupParser();

private:
    std::shared_ptr<HPTNode> parseNode(std::string_view src, std::shared_ptr<HPTNode> parent, size_t& pos, CCNode* parentNode);
    void parseBlock(std::shared_ptr<HPTNode> node, std::string_view src, size_t& pos, CCNode* parentNode);
    void parseEntry(std::shared_ptr<HPTNode> node, std::string_view src, size_t& pos, CCNode* parentNode);
    ErrorInfo getLineColFromPos(std::string_view src, size_t pos);
    std::string parseSelector(std::shared_ptr<HPTNode> node, std::string_view src, size_t& pos);
    std::string parseValue(std::shared_ptr<HPTNode> node, std::string_view src, size_t& pos);
    std::vector<std::string_view> parseArguments(std::shared_ptr<HPTNode> node, std::string_view src, size_t& pos, char startSym = '(', char endSym = ')', int initialDepth = 0);
    std::string_view resolveValue(std::shared_ptr<HPTNode> node, std::string_view rawValue);

    void skipWhitespace(std::string_view src, size_t& pos);
    void skip(std::shared_ptr<HPTNode> parent, std::string_view src, size_t& pos, CCNode* parentNode);
    void skipBlock(std::string_view src, size_t& pos, int depth, bool shouldConsumeClosing = true);
    bool matchChar(std::string_view src, size_t& pos, char c);

    void setVar(std::shared_ptr<HPTNode> node, std::string_view name, std::string_view value);
    Result<std::string_view> getVar(std::shared_ptr<HPTNode> node, std::string_view name);

    CCNode* resolveSelector(CCNode* parent, std::string_view selector);
    void applyAttribute(std::shared_ptr<HPTNode> node, std::string_view key, std::string_view value);
    void runCommand(std::shared_ptr<HPTNode> node, std::string_view key, const std::vector<std::string_view>& args);
    std::string process(std::shared_ptr<HPTNode> node, std::string& value);
    std::string processOnce(std::shared_ptr<HPTNode> node, std::string value);
    HPTNodeTypeDef getNodeType(std::string_view name);

    void handleNode(std::shared_ptr<HPTNode> node, std::string_view src, size_t& pos, CCNode* parentNode);
    void handleCommand(std::shared_ptr<HPTNode> node, std::string_view src, size_t& pos, CCNode* parentNode);
    void handleVariable(std::shared_ptr<HPTNode> node, std::string_view src, size_t& pos, CCNode* parentNode);
    void handleAttribute(std::shared_ptr<HPTNode> node, std::string_view src, size_t& pos, CCNode* parentNode);

    StringMap<AttributeHandler> m_attributeHandlers;
    StringMap<CommandHandler> m_commandHandlers;
    StringMap<MethodHandler> m_methodHandlers;
    StringMap<HPTNodeTypeDef> m_nodeTypes;
};