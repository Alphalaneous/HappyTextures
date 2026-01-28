#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

struct HPTNode;

using AttributeHandler = std::function<void(CCNode*, std::shared_ptr<HPTNode>, ZStringView)>;
using AttributeExportHandler = std::function<ZStringView(CCNode*)>;

using CommandHandler = std::function<void(CCNode*, std::shared_ptr<HPTNode>, const std::vector<ZStringView>&)>;
using MethodHandler = std::function<ZStringView(const std::vector<ZStringView>&)>;

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

    std::shared_ptr<HPTNode> parse(ZStringView src, ZStringView packName, ZStringView identifier, CCNode* rootNode, HPTNode* originalNode = nullptr);

    void registerAttribute(ZStringView name, AttributeHandler handler, AttributeExportHandler exportHandler, bool skipParsing = false);
    void registerCommand(ZStringView name, CommandHandler handler);
    void registerMethod(ZStringView name, MethodHandler handler);
    void registerNodeType(ZStringView name, const HPTNodeTypeDef& def);
    void setupParser();

private:
    std::shared_ptr<HPTNode> parseNode(ZStringView src, std::shared_ptr<HPTNode> parent, size_t& pos, CCNode* parentNode);
    void parseBlock(std::shared_ptr<HPTNode> node, ZStringView src, size_t& pos, CCNode* parentNode);
    void parseEntry(std::shared_ptr<HPTNode> node, ZStringView src, size_t& pos, CCNode* parentNode);
    ErrorInfo getLineColFromPos(ZStringView src, size_t pos);
    std::string parseSelector(std::shared_ptr<HPTNode> node, ZStringView src, size_t& pos);
    std::string parseValue(std::shared_ptr<HPTNode> node, ZStringView src, size_t& pos);
    std::vector<ZStringView> parseArguments(std::shared_ptr<HPTNode> node, ZStringView src, size_t& pos, char startSym = '(', char endSym = ')', int initialDepth = 0);
    ZStringView resolveValue(std::shared_ptr<HPTNode> node, ZStringView rawValue);

    void skipWhitespace(ZStringView src, size_t& pos);
    void skip(std::shared_ptr<HPTNode> parent, ZStringView src, size_t& pos, CCNode* parentNode);
    void skipBlock(ZStringView src, size_t& pos, int depth, bool shouldConsumeClosing = true);
    bool matchChar(ZStringView src, size_t& pos, char c);

    void setVar(std::shared_ptr<HPTNode> node, ZStringView name, ZStringView value);
    Result<ZStringView> getVar(std::shared_ptr<HPTNode> node, ZStringView name);

    CCNode* resolveSelector(CCNode* parent, ZStringView selector);
    void applyAttribute(std::shared_ptr<HPTNode> node, ZStringView key, ZStringView value);
    void runCommand(std::shared_ptr<HPTNode> node, ZStringView key, const std::vector<ZStringView>& args);
    std::string process(std::shared_ptr<HPTNode> node, std::string& value);
    std::string processOnce(std::shared_ptr<HPTNode> node, std::string value);
    HPTNodeTypeDef getNodeType(ZStringView name);

    void handleNode(std::shared_ptr<HPTNode> node, ZStringView src, size_t& pos, CCNode* parentNode);
    void handleCommand(std::shared_ptr<HPTNode> node, ZStringView src, size_t& pos, CCNode* parentNode);
    void handleVariable(std::shared_ptr<HPTNode> node, ZStringView src, size_t& pos, CCNode* parentNode);
    void handleAttribute(std::shared_ptr<HPTNode> node, ZStringView src, size_t& pos, CCNode* parentNode);

    StringMap<AttributeHandler> m_attributeHandlers;
    StringMap<CommandHandler> m_commandHandlers;
    StringMap<MethodHandler> m_methodHandlers;
    StringMap<HPTNodeTypeDef> m_nodeTypes;
};