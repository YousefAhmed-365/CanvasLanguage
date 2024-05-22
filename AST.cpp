#include "headers/AST.hpp"

/* AbstractNode Class */
// Variables
// Constructor & Destructor
// Functions
void AbstractNode::attach(std::shared_ptr<AbstractNode> node){
    m_childrens.emplace_back(node);
}
// Helper Functions
std::vector<std::shared_ptr<AbstractNode>> &AbstractNode::getChildrens(){
    return m_childrens;
}
std::shared_ptr<AbstractNode> AbstractNode::getChild(const unsigned int index) const{
    return m_childrens[index];
}
std::string AbstractNode::getValue(){
    return m_value;
}
void AbstractNode::out(int indent){
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }

    std::cout << "⤷ ● " << m_value << std::endl;

    for (const auto &child : m_childrens) {
        child->out(indent + 1);
    }
}