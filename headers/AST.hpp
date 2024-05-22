#ifndef AST_HPP
#define AST_HPP

#include "CommonLibs.hpp"

enum NodeType{
    NONE
};

struct NodeInfo{
    NodeType type;
    RET_CODE returnCode;

    NodeInfo(NodeType type, RET_CODE code);
};

class AbstractNode{
    protected:
        // Variables
        std::string m_value;
        RET_CODE m_returnCode;
        std::vector<std::shared_ptr<AbstractNode>> m_childrens;
    public:
        // Constructor & Destructor
        virtual ~AbstractNode() = default;
        // Functions
        virtual NodeInfo eval() const = 0;
        void attach(std::shared_ptr<AbstractNode> node);
        // Helper Functions
        std::vector<std::shared_ptr<AbstractNode>> &getChildrens();
        std::shared_ptr<AbstractNode> getChild(const unsigned int index) const;
        std::string getValue();
        void out(int defaultIndent);
};

#endif