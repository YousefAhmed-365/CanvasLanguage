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

void AbstractNode::debug_outNodes(int indent){
    std::string outBuffer;
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }

    std::cout << "⤷ ● " << m_value << std::endl;

    for (const auto &child : m_childrens) {
        child->debug_outNodes(indent + 1);
    }
}

/* AbstractList Struct */
AbstractList::AbstractList(){
    this->info.type = NodeType::ABS_LST;
    this->m_value = "[]";
}

NodeInfo &AbstractList::eval(ScopeManager &scope){
    for(auto &e : m_childrens){
        this->info = e->eval(scope);
    };

    return this->info;
}

/* BlockStatement Struct */
BlockStatement::BlockStatement(){
    this->info.type = NodeType::BLC_STM;
    this->m_value = "_BLOCK";
}

NodeInfo &BlockStatement::eval(ScopeManager &scope){
    scope.pushScope();
    for(auto &e : m_childrens){
        this->info = e->eval(scope);
    };
    scope.popScope();

    return this->info;
}

/* IfStatement Struct */
IfStatement::IfStatement(std::shared_ptr<AbstractNode> condition){
    this->info.type = NodeType::CON_STM;
    this->m_value = "_IF";
    attach(condition);
}

NodeInfo &IfStatement::eval(ScopeManager &scope){

    return this->info;
}

/* WhileStatement Struct */
WhileStatement::WhileStatement(std::shared_ptr<AbstractNode> condition){
    this->info.type = NodeType::WHL_STM;
    this->m_value = "_WHILE";
    attach(condition);
}

NodeInfo &WhileStatement::eval(ScopeManager &scope){
    while(!isVariantEmptyOrNull(identifierToLiteral(m_childrens[0]->eval(scope), scope).data)){
        m_childrens[1]->eval(scope);
    }
    
    return this->info;
}

/* RepeatStatement Struct */
RepeatStatement::RepeatStatement(std::shared_ptr<AbstractNode> count){
    this->info.type = NodeType::REP_STM;
    this->m_value = "_REPEAT";
    attach(count);
}

NodeInfo &RepeatStatement::eval(ScopeManager &scope){
    NodeInfo expression = identifierToLiteral(m_childrens[0]->eval(scope), scope);
    if(expression.type == NodeType::NUM_LIT && variantAsNum(expression.data) >= 0){
        unsigned int count = variantAsNum(expression.data);
        for(unsigned int i = 0; i < count; ++i){
            m_childrens[1]->eval(scope);
        }
    }else{
        throw ParserException("~Error~ Invalid arguments for 'repeat'");
    }
    
    return this->info;
}

/* BinaryExpression Struct */
BinaryExpression::BinaryExpression(std::string &oprStr, std::shared_ptr<AbstractNode> left, std::shared_ptr<AbstractNode> right){
    this->info.type = NodeType::BIN_EXP;
    this->m_value = oprStr;
    this->type = operatorStrToken.at(oprStr);

    attach(left);
    attach(right);
}

NodeInfo &BinaryExpression::eval(ScopeManager &scope){
    NodeInfo leftNode = identifierToLiteral(m_childrens[0]->eval(scope), scope);
    NodeInfo rightNode = identifierToLiteral(m_childrens[1]->eval(scope), scope);

    switch (this->type){
        case OperatorType::OPR_ADD:
            if(leftNode.type == NodeType::NUM_LIT && rightNode.type == NodeType::NUM_LIT){
                this->info.data = variantAsNum(leftNode.data) + variantAsNum(rightNode.data);
                this->info.type = NodeType::NUM_LIT;
            }else if(leftNode.type == NodeType::STR_LIT && rightNode.type == NodeType::STR_LIT){
                this->info.data = '\"' + stripStr(std::get<std::string>(leftNode.data)) + stripStr(std::get<std::string>(rightNode.data)) + '\"';
                this->info.type = NodeType::STR_LIT;
            }else{
                throw ParserException("~Error~ Invalid Binary Operation \'" + variantAsStr(leftNode.data) + ' ' + m_value + ' ' + variantAsStr(rightNode.data) + "\' Incompatible Types.");
            }
            break;
        case OperatorType::OPR_SUB:
            if(leftNode.type == NodeType::NUM_LIT && rightNode.type == NodeType::NUM_LIT){
                this->info.data = variantAsNum(leftNode.data) - variantAsNum(rightNode.data);
                this->info.type = NodeType::NUM_LIT;
            }else{
                throw ParserException("~Error~ Invalid Binary Operation \'" + variantAsStr(leftNode.data) + ' ' + m_value + ' ' + variantAsStr(rightNode.data) + "\' Incompatible Types.");
            }
            break;
        case OperatorType::OPR_MUL:
            if(leftNode.type == NodeType::NUM_LIT && rightNode.type == NodeType::NUM_LIT){
                this->info.data = variantAsNum(leftNode.data) * variantAsNum(rightNode.data);
                this->info.type = NodeType::NUM_LIT;
            }else{
                throw ParserException("~Error~ Invalid Binary Operation \'" + variantAsStr(leftNode.data) + ' ' + m_value + ' ' + variantAsStr(rightNode.data) + "\' Incompatible Types.");
            }
            break;
        case OperatorType::OPR_DIV:
            if(leftNode.type == NodeType::NUM_LIT && rightNode.type == NodeType::NUM_LIT){
                this->info.data = variantAsNum(leftNode.data) / variantAsNum(rightNode.data);
                this->info.type = NodeType::NUM_LIT;
            }else{
                throw ParserException("~Error~ Invalid Binary Operation \'" + variantAsStr(leftNode.data) + ' ' + m_value + ' ' + variantAsStr(rightNode.data) + "\' Incompatible Types.");
            }
            break;
        case OperatorType::OPR_MOD:
            if(leftNode.type == NodeType::NUM_LIT && rightNode.type == NodeType::NUM_LIT){
                this->info.data = static_cast<int>(variantAsNum(leftNode.data)) % static_cast<int>(variantAsNum(rightNode.data));
                this->info.type = NodeType::NUM_LIT;
            }else{
                throw ParserException("~Error~ Invalid Binary Operation \'" + variantAsStr(leftNode.data) + ' ' + m_value + ' ' + variantAsStr(rightNode.data) + "\' Incompatible Types.");
            }
            break;
        case OperatorType::OPR_EXP:
            if(leftNode.type == NodeType::NUM_LIT && rightNode.type == NodeType::NUM_LIT){
                this->info.data = std::pow(variantAsNum(leftNode.data), variantAsNum(rightNode.data));
                this->info.type = NodeType::NUM_LIT;
            }else{
                throw ParserException("~Error~ Invalid Binary Operation \'" + variantAsStr(leftNode.data) + ' ' + m_value + ' ' + variantAsStr(rightNode.data) + "\' Incompatible Types.");
            }
            break;
        case OperatorType::LOG_EQL:
            if(leftNode.type == NodeType::NUM_LIT && rightNode.type == NodeType::NUM_LIT){
                this->info.data = variantAsNum(leftNode.data) == variantAsNum(rightNode.data);
                this->info.type = NodeType::NUM_LIT;
            }else if(leftNode.type == NodeType::STR_LIT && rightNode.type == NodeType::STR_LIT){
                this->info.data = std::get<std::string>(leftNode.data) == std::get<std::string>(rightNode.data);
            }else{
                throw ParserException("~Error~ Invalid Binary Operation \'" + variantAsStr(leftNode.data) + ' ' + m_value + ' ' + variantAsStr(rightNode.data) + "\' Incompatible Types.");
            }
            break;
        case OperatorType::LOG_NEQ:
            if(leftNode.type == NodeType::NUM_LIT && rightNode.type == NodeType::NUM_LIT){
                this->info.data = variantAsNum(leftNode.data) != variantAsNum(rightNode.data);
                this->info.type = NodeType::NUM_LIT;
            }else if(leftNode.type == NodeType::STR_LIT && rightNode.type == NodeType::STR_LIT){
                this->info.data = std::get<std::string>(leftNode.data) != std::get<std::string>(rightNode.data);
            }else{
                throw ParserException("~Error~ Invalid Binary Operation \'" + variantAsStr(leftNode.data) + ' ' + m_value + ' ' + variantAsStr(rightNode.data) + "\' Incompatible Types.");
            }
            break;
        case OperatorType::LOG_GEQ:
            if(leftNode.type == NodeType::NUM_LIT && rightNode.type == NodeType::NUM_LIT){
                this->info.data = leftNode.data >= rightNode.data;
                this->info.type = NodeType::NUM_LIT;
            }else{
                throw ParserException("~Error~ Invalid Binary Operation \'" + variantAsStr(leftNode.data) + ' ' + m_value + ' ' + variantAsStr(rightNode.data) + "\' Incompatible Types.");
            }
            break;
        case OperatorType::LOG_GRE:
            if(leftNode.type == NodeType::NUM_LIT && rightNode.type == NodeType::NUM_LIT){
                this->info.data = leftNode.data > rightNode.data;
                this->info.type = NodeType::NUM_LIT;
            }else{
                throw ParserException("~Error~ Invalid Binary Operation \'" + variantAsStr(leftNode.data) + ' ' + m_value + ' ' + variantAsStr(rightNode.data) + "\' Incompatible Types.");
            }
            break;
        case OperatorType::LOG_LEQ:
            if(leftNode.type == NodeType::NUM_LIT && rightNode.type == NodeType::NUM_LIT){
                this->info.data = leftNode.data <= rightNode.data;
                this->info.type = NodeType::NUM_LIT;
            }else{
                throw ParserException("~Error~ Invalid Binary Operation \'" + variantAsStr(leftNode.data) + ' ' + m_value + ' ' + variantAsStr(rightNode.data) + "\' Incompatible Types.");
            }
            break;
        case OperatorType::LOG_LES:
            if(leftNode.type == NodeType::NUM_LIT && rightNode.type == NodeType::NUM_LIT){
                this->info.data = leftNode.data < rightNode.data;
                this->info.type = NodeType::NUM_LIT;
            }else{
                throw ParserException("~Error~ Invalid Binary Operation \'" + variantAsStr(leftNode.data) + ' ' + m_value + ' ' + variantAsStr(rightNode.data) + "\' Incompatible Types.");
            }
            break;
        case OperatorType::LOG_AND:
            if(leftNode.type == NodeType::NUM_LIT && rightNode.type == NodeType::NUM_LIT){
                this->info.data = variantAsNum(leftNode.data) && variantAsNum(rightNode.data);
                this->info.type = NodeType::NUM_LIT;
            }else{
                throw ParserException("~Error~ Invalid Binary Operation \'" + variantAsStr(leftNode.data) + ' ' + m_value + ' ' + variantAsStr(rightNode.data) + "\' Incompatible Types.");
            }
            break;
        case OperatorType::LOG_LOR:
            if(leftNode.type == NodeType::NUM_LIT && rightNode.type == NodeType::NUM_LIT){
                this->info.data = variantAsNum(leftNode.data) || variantAsNum(rightNode.data);
                this->info.type = NodeType::NUM_LIT;
            }else{
                throw ParserException("~Error~ Invalid Binary Operation \'" + variantAsStr(leftNode.data) + ' ' + m_value + ' ' + variantAsStr(rightNode.data) + "\' Incompatible Types.");
            }
            break;
        default:
            throw ParserException("~Error~ Invalid Binary Operation \'" + m_value + "\'");
            break;
    }

    return this->info;
}

/* UnaryExpression Struct */
UnaryExpression::UnaryExpression(std::string &oprStr, std::shared_ptr<AbstractNode> left){
    this->info.type = NodeType::UNR_EXP;
    this->type = operatorStrToken.at(oprStr);
    this->m_value = oprStr;

    attach(left);
}

NodeInfo &UnaryExpression::eval(ScopeManager &scope){
    NodeInfo &leftNode = m_childrens[0]->eval(scope);

    switch (this->type){
    case OperatorType::LOG_NOT:
        if(leftNode.type == NodeType::NUM_LIT){
                this->info.data = !variantAsNum(leftNode.data);
        }else{
            throw ParserException("~Error~ Invalid Unary Operation \'" + m_value + variantAsStr(leftNode.data) + "\' Incompatible Type.");
        }
        break;

    case OperatorType::OPR_SUB:
        if(leftNode.type == NodeType::NUM_LIT){
            this->info.data = -variantAsNum(leftNode.data);
        }else{
            throw ParserException("~Error~ Invalid Unary Operation \'" + m_value + variantAsStr(leftNode.data) + "\' Incompatible Type.");
        }
        break;
    
    default:
        break;
    }

    this->info.type = leftNode.type;
    return this->info;
}

/* Literal Struct */
Literal::Literal(Data &value){
    this->m_value = variantAsStr(value);

    this->info.data = value;
}

NodeInfo &Literal::eval(ScopeManager &scope){

    return this->info;
}

/* Identifier Struct */
Identifier::Identifier(std::string &name){
    this->info.type = NodeType::IDN;
    this->info.data = name;
    this->m_value = name;
}

NodeInfo &Identifier::eval(ScopeManager &scope){
    
    return this->info;
}

/* DefStatement Struct */
DefStatement::DefStatement(){
    this->info.type = NodeType::DEF_STM;
    this->m_value = "_DEF";
}

NodeInfo &DefStatement::eval(ScopeManager &scope){
    
    return this->info;
}

/* RetStatement Struct */
RetStatement::RetStatement(std::shared_ptr<AbstractNode> expression){
    this->info.type = NodeType::RET_STM;
    this->m_value = "_RET";
    attach(expression);
}

NodeInfo &RetStatement::eval(ScopeManager &scope){

    return this->info;
}

/* CallStatement Struct */
CallStatement::CallStatement(std::string &name, std::shared_ptr<AbstractNode> argsList){
    this->info.type = NodeType::CAL_STM;
    this->m_value = "_CALL";
    attach(std::make_shared<Identifier>(name));
    attach(argsList);
}

NodeInfo &CallStatement::eval(ScopeManager &scope){
    std::string &identifier = std::get<std::string>(m_childrens[0]->eval(scope).data);
    std::vector<NodeInfo> argsList;
    argsList.reserve(m_childrens[1]->getChildrens().size());

    for(auto &e : m_childrens[1]->getChildrens()){
        argsList.emplace_back(identifierToLiteral(e->eval(scope), scope));
    }
    
    if(identifier == "print"){
        for(auto &e : argsList){
            std::string toBePrinted = variantAsStr(e.data);
            std::cout << sanitizeStr(toBePrinted) << std::endl;
        }
    }

    return this->info;
}

/* AssignementStatment Struct */
AssignementStatment::AssignementStatment(std::string &oprStr, std::string &identifier, std::shared_ptr<AbstractNode> expression){
    this->info.type = NodeType::ASG_STM;
    this->m_value = oprStr;
    attach(std::make_shared<Identifier>(identifier));
    attach(expression);
}

NodeInfo &AssignementStatment::eval(ScopeManager &scope){
    std::string &identifier = std::get<std::string>(m_childrens[0]->eval(scope).data);
    NodeInfo expression = identifierToLiteral(m_childrens[1]->eval(scope), scope);

    Data *data = scope.findData(identifier);

    if(data == nullptr){
        if(this->m_value == "="){
            scope.pushData(identifier, expression.data);
        }else{
            throw ParserException("~Error~ Undefined identifier \'" + identifier + "\'.");
        }
    }else{
        if(this->m_value == "="){
            *data = expression.data;
        }
    }
    
    return this->info;
}

// Helper Functions
NodeInfo identifierToLiteral(NodeInfo &info, ScopeManager &scope){
    Data *data;
    switch(info.type){
    case NodeType::IDN:
        {
            if(Data *dataPtr = scope.findData(std::get<std::string>(info.data))){
                data = dataPtr;
            }else{
                throw ParserException("~Error~ Undefined identifier \'" + std::get<std::string>(info.data) + "\'.");
            }
        }
        break;
    default:
        return info;
        break;
    }

    if(std::holds_alternative<int32_t>(*data) || std::holds_alternative<float>(*data)){
        return NodeInfo(NodeType::NUM_LIT, *data);
    }

    return NodeInfo(NodeType::STR_LIT, *data);
}