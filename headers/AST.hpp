#ifndef AST_HPP
#define AST_HPP

#include "CommonLibs.hpp"

enum class NodeType{
    NONE,

    BIN_EXP,
    UNR_EXP,

    LIT,
    NUM_LIT,
    STR_LIT
};

struct NodeInfo{
    // Variables
    NodeType type;
    Data data;
    RET_CODE returnCode;

    // Constructor & Destructor
    NodeInfo();
    NodeInfo(NodeType type, Data value, RET_CODE returnCode);
    ~NodeInfo() = default;
};

class AbstractNode{
    protected:
        // Variables
        std::string m_value;
        RET_CODE m_returnCode;
        std::vector<std::shared_ptr<AbstractNode>> m_childrens;
    public:
        // Variables
        NodeInfo info;
        // Constructor & Destructor
        virtual ~AbstractNode() = default;
        // Functions
        virtual NodeInfo eval() = 0;
        void attach(std::shared_ptr<AbstractNode> node);
        // Helper Functions
        std::vector<std::shared_ptr<AbstractNode>> &getChildrens();
        std::shared_ptr<AbstractNode> getChild(const unsigned int index) const;
        std::string getValue();
        void setValue(std::string value);
        void out(int defaultIndent);
};

struct AbstractList : public AbstractNode{
    AbstractList();
    ~AbstractList() = default;

    NodeInfo eval() override;
};

struct BinaryExpression : public AbstractNode{
    OperatorType type;

    BinaryExpression(std::string &oprStr, std::shared_ptr<AbstractNode> left, std::shared_ptr<AbstractNode> right);
    ~BinaryExpression() = default;

    NodeInfo eval() override;
};

struct IfStatement : public AbstractNode{
    IfStatement(std::shared_ptr<AbstractNode> condition);
    ~IfStatement() = default;

    NodeInfo eval() override;
};

struct WhileStatement : public AbstractNode{
    WhileStatement(std::shared_ptr<AbstractNode> condition);
    ~WhileStatement() = default;

    NodeInfo eval() override;
};

struct RepeatStatement : public AbstractNode{
    RepeatStatement(std::shared_ptr<AbstractNode> count);
    ~RepeatStatement() = default;

    NodeInfo eval() override;
};

struct UnaryExpression : public AbstractNode{
    OperatorType type;

    UnaryExpression(std::string &oprStr, std::shared_ptr<AbstractNode> left);
    ~UnaryExpression() = default;

    NodeInfo eval() override;
};

struct Literal : public AbstractNode{
    Data value;

    Literal(Data &value);
    ~Literal() = default;

    NodeInfo eval() override;
};

struct Identifier : public AbstractNode{
    Identifier(std::string &name);
    ~Identifier() = default;

    NodeInfo eval() override;
};

struct DefStatement : public AbstractNode{
    DefStatement();
    ~DefStatement() = default;

    NodeInfo eval() override;
};

struct RetStatement : public AbstractNode{
    RetStatement(std::shared_ptr<AbstractNode> expression);
    ~RetStatement() = default;

    NodeInfo eval() override;
};

#endif