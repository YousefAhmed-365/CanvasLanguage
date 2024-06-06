#include "headers/AST.hpp"
/* NodeInfo Struct */
// Constructor & Destructor
NodeInfo::NodeInfo(){
    type = NodeType::NONE;
}

NodeInfo::NodeInfo(NodeType type, Data value){
    this->type = type;
    this->data = value;
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

void AbstractNode::setValue(std::string value){
    m_value = value;
}

void AbstractNode::out(int indent){
    std::string outBuffer;
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }

    std::cout << "⤷ ● " << m_value << std::endl;

    for (const auto &child : m_childrens) {
        child->out(indent + 1);
    }
}

/* AbstractList Struct */
AbstractList::AbstractList(){
    this->info.type = NodeType::ABS_LST;
    this->m_value = "[]";
}

NodeInfo AbstractList::eval(){

    return this->info;
}

/* IfStatement Struct */
IfStatement::IfStatement(std::shared_ptr<AbstractNode> condition){
    this->info.type = NodeType::CON_STM;
    this->m_value = "_IF";
    attach(condition);
}

NodeInfo IfStatement::eval(){

    return this->info;
}

/* WhileStatement Struct */
WhileStatement::WhileStatement(std::shared_ptr<AbstractNode> condition){
    this->info.type = NodeType::WHL_STM;
    this->m_value = "_WHILE";
    attach(condition);
}

NodeInfo WhileStatement::eval(){

    return this->info;
}

/* RepeatStatement Struct */
RepeatStatement::RepeatStatement(std::shared_ptr<AbstractNode> count){
    this->info.type = NodeType::REP_STM;
    this->m_value = "_REPEAT";
    attach(count);
}

NodeInfo RepeatStatement::eval(){

    return this->info;
}

/* BinaryExpression Struct */
BinaryExpression::BinaryExpression(std::string &oprStr, std::shared_ptr<AbstractNode> left, std::shared_ptr<AbstractNode> right){
    this->info.type = NodeType::BIN_EXP;
    this->m_value = oprStr;

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

/* UnaryExpression Struct */
UnaryExpression::UnaryExpression(std::string &oprStr, std::shared_ptr<AbstractNode> left){
    this->info.type = NodeType::UNR_EXP;
    this->m_value = oprStr;

    attach(left);
}

NodeInfo UnaryExpression::eval(){
    NodeInfo leftNode = m_childrens[0]->eval();

    return this->info;
}

/* Literal Struct */
Literal::Literal(Data &value){
    this->info.type = NodeType::LIT;
    this->m_value = std::get<std::string>(value);
    this->value = value;
    
    this->info.type = std::holds_alternative<int32_t>(value) || std::holds_alternative<float>(value)? NodeType::NUM_LIT : NodeType::STR_LIT;
    this->info.data = value;
}

NodeInfo Literal::eval(){

    return this->info;
}

/* Identifier Struct */
Identifier::Identifier(std::string &name){
    this->info.type = NodeType::IDN;
    this->m_value = name;
}

NodeInfo Identifier::eval(){

    return this->info;
}

/* DefStatement Struct */
DefStatement::DefStatement(){
    this->info.type = NodeType::DEF_STM;
    this->m_value = "_DEF";
}

NodeInfo DefStatement::eval(){
    
    return this->info;
}

/* RetStatement Struct */
RetStatement::RetStatement(std::shared_ptr<AbstractNode> expression){
    this->info.type = NodeType::RET_STM;
    this->m_value = "_RET";
    attach(expression);
}

NodeInfo RetStatement::eval(){

    return this->info;
}

/* CallStatement Struct */
CallStatement::CallStatement(std::string &name, std::shared_ptr<AbstractNode> argsList){
    this->info.type = NodeType::CAL_STM;
    this->m_value = "_CALL";
    attach(std::make_shared<Identifier>(name));
    attach(argsList);
}

NodeInfo CallStatement::eval(){

    return this->info;
}

/* AssignementStatment Struct */
AssignementStatment::AssignementStatment(std::string &identifier, std::shared_ptr<AbstractNode> expression){
    this->m_value = "=";
    attach(std::make_shared<Identifier>(identifier));
    attach(expression);
}

NodeInfo AssignementStatment::eval(){

    return this->info;
}