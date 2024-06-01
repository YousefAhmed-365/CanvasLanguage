#ifndef AST_HPP
#define AST_HPP

#include "CommonLibs.hpp"

enum class NodeType{
    NONE,

    BIN_EXP,

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
        void out(int defaultIndent);
};

struct BinaryExpression : public AbstractNode{
    OperatorType type;

    BinaryExpression(std::string &oprStr, std::shared_ptr<AbstractNode> left, std::shared_ptr<AbstractNode> right);
    ~BinaryExpression() = default;

    NodeInfo eval() override;
};

struct Literal : public AbstractNode{
    Data value;

    Literal(Data &value);
    ~Literal() = default;

    NodeInfo eval() override;
};

#endif