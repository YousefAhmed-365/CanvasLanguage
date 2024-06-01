#include "headers/AST.hpp"
/* NodeInfo Struct */
// Constructor & Destructor
NodeInfo::NodeInfo(){
    type = NodeType::NONE;
    returnCode = RET_CODE::NONE;
}
NodeInfo::NodeInfo(NodeType type, Data value, RET_CODE returnCode){
    this->type = type;
    this->data = value;
    this->returnCode = returnCode;
}

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

/* BinaryExpression Struct */
BinaryExpression::BinaryExpression(std::string &oprStr, std::shared_ptr<AbstractNode> left, std::shared_ptr<AbstractNode> right){
    this->m_value = oprStr;
    this->info.type = NodeType::BIN_EXP;

    attach(left);
    attach(right);
}

NodeInfo BinaryExpression::eval(){
    NodeInfo leftNode = m_childrens[0]->eval();
    NodeInfo rightNode = m_childrens[1]->eval();

    switch (this->type){
        case OperatorType::OPR_ADD:
            if(leftNode.type == NodeType::NUM_LIT && rightNode.type == NodeType::NUM_LIT){
                this->info.data = variantAsNum(leftNode.data) + variantAsNum(rightNode.data);
            }
            break;
        case OperatorType::OPR_SUB:
            break;
        case OperatorType::OPR_MUL:
            break;
        case OperatorType::OPR_DIV:
            break;
        case OperatorType::OPR_MOD:
            break;
        case OperatorType::OPR_EXP:
            break;
        case OperatorType::LOG_EQL:
            break;
        case OperatorType::LOG_NEQ:
            break;
        case OperatorType::LOG_GEQ:
            break;
        case OperatorType::LOG_GRE:
            break;
        case OperatorType::LOG_LEQ:
            break;
        case OperatorType::LOG_LES:
            break;
        case OperatorType::LOG_AND:
            break;
        case OperatorType::LOG_LOR:
            break;
        case OperatorType::LOG_NOT:
            break;
        default:
            break;
    }

    return this->info;
}

/* Literal Struct */
Literal::Literal(Data &value){
    this->value = value;
    
    this->info.type = std::holds_alternative<int32_t>(value) || std::holds_alternative<float>(value)? NodeType::NUM_LIT : NodeType::STR_LIT;
    this->info.data = value;
}

NodeInfo Literal::eval(){
    return this->info;
}