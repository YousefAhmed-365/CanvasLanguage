#ifndef AST_HPP
#define AST_HPP

#include "CommonLibs.hpp"
#include "ResManager.hpp"
#include "Error.hpp"

enum class NodeType{
    NONE,

    BLC_STM,
    ABS_LST,
    BIN_EXP,
    UNR_EXP,
    CON_STM,
    WHL_STM,
    REP_STM,
    DEF_STM,
    RET_STM,
    CAL_STM,
    ASG_STM,

    IDN,
    LIT,
    NUM_LIT,
    STR_LIT
};

struct NodeInfo{
    // Variables
    NodeType type;
    Data data;

    // Constructor & Destructor
    NodeInfo();
    NodeInfo(NodeType type, Data value);
    ~NodeInfo() = default;
};

class AbstractNode{
    protected:
        // Variables
        std::string m_value;
        std::vector<std::shared_ptr<AbstractNode>> m_childrens;
    public:
        // Variables
        NodeInfo info;

        // Constructor & Destructor
        virtual ~AbstractNode() = default;

        // Functions
        virtual NodeInfo &eval(ScopeManager &scope) = 0;
        void attach(std::shared_ptr<AbstractNode> node);
        
        // Helper Functions
        std::vector<std::shared_ptr<AbstractNode>> &getChildrens();
        std::shared_ptr<AbstractNode> getChild(const unsigned int index) const;
        std::string getValue();
        void setValue(std::string value);
        void debug_outNodes(int defaultIndent);
};


struct AbstractList : public AbstractNode{
    AbstractList();
    ~AbstractList() = default;

    NodeInfo &eval(ScopeManager &scope) override;
};

struct BlockStatement : public AbstractNode{
    BlockStatement();
    ~BlockStatement() = default;

    NodeInfo &eval(ScopeManager &scope) override;
};

struct BinaryExpression : public AbstractNode{
    OperatorType type;

    BinaryExpression(std::string &oprStr, std::shared_ptr<AbstractNode> left, std::shared_ptr<AbstractNode> right);
    ~BinaryExpression() = default;

    NodeInfo &eval(ScopeManager &scope) override;
};

struct UnaryExpression : public AbstractNode{
    OperatorType type;
    
    UnaryExpression(std::string &oprStr, std::shared_ptr<AbstractNode> left);
    ~UnaryExpression() = default;

    NodeInfo &eval(ScopeManager &scope) override;
};

struct IfStatement : public AbstractNode{
    IfStatement(std::shared_ptr<AbstractNode> condition);
    ~IfStatement() = default;

    NodeInfo &eval(ScopeManager &scope) override;
};

struct WhileStatement : public AbstractNode{
    WhileStatement(std::shared_ptr<AbstractNode> condition);
    ~WhileStatement() = default;

    NodeInfo &eval(ScopeManager &scope) override;
};

struct RepeatStatement : public AbstractNode{
    RepeatStatement(std::shared_ptr<AbstractNode> count);
    ~RepeatStatement() = default;

    NodeInfo &eval(ScopeManager &scope) override;
};

struct Literal : public AbstractNode{
    Literal(Data &value);
    ~Literal() = default;

    NodeInfo &eval(ScopeManager &scope) override;
};

struct Identifier : public AbstractNode{
    Identifier(std::string &name);
    ~Identifier() = default;

    NodeInfo &eval(ScopeManager &scope) override;
};

struct DefStatement : public AbstractNode{
    DefStatement();
    ~DefStatement() = default;

    NodeInfo &eval(ScopeManager &scope) override;
};

struct RetStatement : public AbstractNode{
    RetStatement(std::shared_ptr<AbstractNode> expression);
    ~RetStatement() = default;

    NodeInfo &eval(ScopeManager &scope) override;
};

struct CallStatement : public AbstractNode{
    CallStatement(std::string &name, std::shared_ptr<AbstractNode> argsList);
    ~CallStatement() = default;

    NodeInfo &eval(ScopeManager &scope) override;
};

struct AssignementStatment : public AbstractNode{    
    AssignementStatment(std::string &oprStr, std::string &identifier, std::shared_ptr<AbstractNode> expression);
    ~AssignementStatment() = default;

    NodeInfo &eval(ScopeManager &scope) override;
};

// Helper Functions
NodeInfo identifierToLiteral(NodeInfo &info, ScopeManager &scope);

#endif