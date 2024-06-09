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

NodeInfo AbstractList::eval(ScopeManager &scope){
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

NodeInfo BlockStatement::eval(ScopeManager &scope){
    scope.pushScope();
    for(auto &e : m_childrens){
        e->eval(scope);
    };
    scope.popScope();

    return this->info;
}

/* PostBlockStatement Struct */
PostBlockStatement::PostBlockStatement(){
    this->info.type = NodeType::BLC_STM;
    this->m_value = "_BLOCK_POST";
}

NodeInfo PostBlockStatement::eval(ScopeManager &scope){
    for(auto &e : m_childrens){
        NodeInfo _info = e->eval(scope);
        if(_info.type == NodeType::BRK_STM || _info.type == NodeType::CON_STM || e->info.type == NodeType::RET_STM){
            return _info;
        }
    };

    return this->info;
}

/* IfStatement Struct */
IfStatement::IfStatement(std::shared_ptr<AbstractNode> condition){
    this->info.type = NodeType::IFC_STM;
    this->m_value = "_IF";
    attach(condition);
}

NodeInfo IfStatement::eval(ScopeManager &scope){
    if(!isVariantEmptyOrNull(identifierToLiteral(m_childrens[0]->eval(scope), scope).data)){
        return m_childrens[1]->eval(scope);
    }else{
        bool foundTruthy = false;
        for(auto &e : m_childrens[2]->getChildrens()){
            if(!isVariantEmptyOrNull(identifierToLiteral(e->getChild(0)->eval(scope), scope).data)){
                foundTruthy = true;
                return e->eval(scope);
                break;
            }
        }

        if(!foundTruthy && m_childrens.size() == 4){
            return m_childrens[3]->eval(scope);
        }
    }
    
    return this->info;
}

/* WhileStatement Struct */
WhileStatement::WhileStatement(std::shared_ptr<AbstractNode> condition){
    this->info.type = NodeType::WHL_STM;
    this->m_value = "_WHILE";
    attach(condition);
}

NodeInfo WhileStatement::eval(ScopeManager &scope){
    scope.pushScope();
    while(!isVariantEmptyOrNull(identifierToLiteral(m_childrens[0]->eval(scope), scope).data)){
        NodeInfo _info = m_childrens[1]->eval(scope);
        if(_info.type == NodeType::BRK_STM){
            break;
        }else if(_info.type == NodeType::CON_STM){
            continue;
        }
    }
    scope.popScope();
    
    return this->info;
}

/* RepeatStatement Struct */
RepeatStatement::RepeatStatement(std::shared_ptr<AbstractNode> count){
    this->info.type = NodeType::REP_STM;
    this->m_value = "_REPEAT";
    attach(count);
}

NodeInfo RepeatStatement::eval(ScopeManager &scope){
    NodeInfo expression = identifierToLiteral(m_childrens[0]->eval(scope), scope);
    if(expression.type == NodeType::NUM_LIT && variantAsNum(expression.data) >= 0){
        unsigned int count = variantAsNum(expression.data);
        for(unsigned int i = 0; i < count; ++i){
            NodeInfo _info = m_childrens[1]->eval(scope);
            if(_info.type == NodeType::BRK_STM){
                break;
            }else if(_info.type == NodeType::CON_STM){
                continue;
            }
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

NodeInfo BinaryExpression::eval(ScopeManager &scope){
    NodeInfo leftNode = identifierToLiteral(m_childrens[0]->eval(scope), scope);
    NodeInfo rightNode = identifierToLiteral(m_childrens[1]->eval(scope), scope);

    switch (this->type){
        case OperatorType::OPR_ADD:
            if(leftNode.type == NodeType::NUM_LIT && rightNode.type == NodeType::NUM_LIT){
                leftNode.data = variantAsNum(leftNode.data) + variantAsNum(rightNode.data);
            }else if(leftNode.type == NodeType::STR_LIT && rightNode.type == NodeType::STR_LIT){
                leftNode.data = '\"' + stripStr(std::get<std::string>(leftNode.data)) + stripStr(std::get<std::string>(rightNode.data)) + '\"';
            }else{
                throw ParserException("~Error~ Invalid Binary Operation \'" + variantAsStr(leftNode.data) + ' ' + m_value + ' ' + variantAsStr(rightNode.data) + "\' Incompatible Types.");
            }
            break;
        case OperatorType::OPR_SUB:
            if(leftNode.type == NodeType::NUM_LIT && rightNode.type == NodeType::NUM_LIT){
                leftNode.data = variantAsNum(leftNode.data) - variantAsNum(rightNode.data);
            }else{
                throw ParserException("~Error~ Invalid Binary Operation \'" + variantAsStr(leftNode.data) + ' ' + m_value + ' ' + variantAsStr(rightNode.data) + "\' Incompatible Types.");
            }
            break;
        case OperatorType::OPR_MUL:
            if(leftNode.type == NodeType::NUM_LIT && rightNode.type == NodeType::NUM_LIT){
                leftNode.data = variantAsNum(leftNode.data) * variantAsNum(rightNode.data);
            }else{
                throw ParserException("~Error~ Invalid Binary Operation \'" + variantAsStr(leftNode.data) + ' ' + m_value + ' ' + variantAsStr(rightNode.data) + "\' Incompatible Types.");
            }
            break;
        case OperatorType::OPR_DIV:
            if(leftNode.type == NodeType::NUM_LIT && rightNode.type == NodeType::NUM_LIT){
                leftNode.data = variantAsNum(leftNode.data) / variantAsNum(rightNode.data);
            }else{
                throw ParserException("~Error~ Invalid Binary Operation \'" + variantAsStr(leftNode.data) + ' ' + m_value + ' ' + variantAsStr(rightNode.data) + "\' Incompatible Types.");
            }
            break;
        case OperatorType::OPR_MOD:
            if(leftNode.type == NodeType::NUM_LIT && rightNode.type == NodeType::NUM_LIT){
                leftNode.data = static_cast<int>(variantAsNum(leftNode.data)) % static_cast<int>(variantAsNum(rightNode.data));
            }else{
                throw ParserException("~Error~ Invalid Binary Operation \'" + variantAsStr(leftNode.data) + ' ' + m_value + ' ' + variantAsStr(rightNode.data) + "\' Incompatible Types.");
            }
            break;
        case OperatorType::OPR_EXP:
            if(leftNode.type == NodeType::NUM_LIT && rightNode.type == NodeType::NUM_LIT){
                leftNode.data = std::pow(variantAsNum(leftNode.data), variantAsNum(rightNode.data));
            }else{
                throw ParserException("~Error~ Invalid Binary Operation \'" + variantAsStr(leftNode.data) + ' ' + m_value + ' ' + variantAsStr(rightNode.data) + "\' Incompatible Types.");
            }
            break;
        case OperatorType::LOG_EQL:
            if(leftNode.type == NodeType::NUM_LIT && rightNode.type == NodeType::NUM_LIT){
                leftNode.data = variantAsNum(leftNode.data) == variantAsNum(rightNode.data);
            }else if(leftNode.type == NodeType::STR_LIT && rightNode.type == NodeType::STR_LIT){
                leftNode.data = std::get<std::string>(leftNode.data) == std::get<std::string>(rightNode.data);
            }else{
                throw ParserException("~Error~ Invalid Binary Operation \'" + variantAsStr(leftNode.data) + ' ' + m_value + ' ' + variantAsStr(rightNode.data) + "\' Incompatible Types.");
            }
            break;
        case OperatorType::LOG_NEQ:
            if(leftNode.type == NodeType::NUM_LIT && rightNode.type == NodeType::NUM_LIT){
                leftNode.data = variantAsNum(leftNode.data) != variantAsNum(rightNode.data);
            }else if(leftNode.type == NodeType::STR_LIT && rightNode.type == NodeType::STR_LIT){
                leftNode.data = std::get<std::string>(leftNode.data) != std::get<std::string>(rightNode.data);
            }else{
                throw ParserException("~Error~ Invalid Binary Operation \'" + variantAsStr(leftNode.data) + ' ' + m_value + ' ' + variantAsStr(rightNode.data) + "\' Incompatible Types.");
            }
            break;
        case OperatorType::LOG_GEQ:
            if(leftNode.type == NodeType::NUM_LIT && rightNode.type == NodeType::NUM_LIT){
                leftNode.data = leftNode.data >= rightNode.data;
            }else{
                throw ParserException("~Error~ Invalid Binary Operation \'" + variantAsStr(leftNode.data) + ' ' + m_value + ' ' + variantAsStr(rightNode.data) + "\' Incompatible Types.");
            }
            break;
        case OperatorType::LOG_GRE:
            if(leftNode.type == NodeType::NUM_LIT && rightNode.type == NodeType::NUM_LIT){
                leftNode.data = leftNode.data > rightNode.data;
            }else{
                throw ParserException("~Error~ Invalid Binary Operation \'" + variantAsStr(leftNode.data) + ' ' + m_value + ' ' + variantAsStr(rightNode.data) + "\' Incompatible Types.");
            }
            break;
        case OperatorType::LOG_LEQ:
            if(leftNode.type == NodeType::NUM_LIT && rightNode.type == NodeType::NUM_LIT){
                leftNode.data = leftNode.data <= rightNode.data;
            }else{
                throw ParserException("~Error~ Invalid Binary Operation \'" + variantAsStr(leftNode.data) + ' ' + m_value + ' ' + variantAsStr(rightNode.data) + "\' Incompatible Types.");
            }
            break;
        case OperatorType::LOG_LES:
            if(leftNode.type == NodeType::NUM_LIT && rightNode.type == NodeType::NUM_LIT){
                leftNode.data = leftNode.data < rightNode.data;
            }else{
                throw ParserException("~Error~ Invalid Binary Operation \'" + variantAsStr(leftNode.data) + ' ' + m_value + ' ' + variantAsStr(rightNode.data) + "\' Incompatible Types.");
            }
            break;
        case OperatorType::LOG_AND:
            if(leftNode.type == NodeType::NUM_LIT && rightNode.type == NodeType::NUM_LIT){
                leftNode.data = variantAsNum(leftNode.data) && variantAsNum(rightNode.data);
            }else{
                throw ParserException("~Error~ Invalid Binary Operation \'" + variantAsStr(leftNode.data) + ' ' + m_value + ' ' + variantAsStr(rightNode.data) + "\' Incompatible Types.");
            }
            break;
        case OperatorType::LOG_LOR:
            if(leftNode.type == NodeType::NUM_LIT && rightNode.type == NodeType::NUM_LIT){
                leftNode.data = variantAsNum(leftNode.data) || variantAsNum(rightNode.data);
            }else{
                throw ParserException("~Error~ Invalid Binary Operation \'" + variantAsStr(leftNode.data) + ' ' + m_value + ' ' + variantAsStr(rightNode.data) + "\' Incompatible Types.");
            }
            break;
        default:
            throw ParserException("~Error~ Invalid Binary Operation \'" + m_value + "\'");
            break;
    }

    return leftNode;
}

/* UnaryExpression Struct */
UnaryExpression::UnaryExpression(std::string &oprStr, std::shared_ptr<AbstractNode> left){
    this->info.type = NodeType::UNR_EXP;
    this->type = operatorStrToken.at(oprStr);
    this->m_value = oprStr;

    attach(left);
}

NodeInfo UnaryExpression::eval(ScopeManager &scope){
    NodeInfo leftNode = m_childrens[0]->eval(scope);

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

NodeInfo Literal::eval(ScopeManager &scope){

    return this->info;
}

/* Identifier Struct */
Identifier::Identifier(std::string &name){
    this->info.type = NodeType::IDN;
    this->info.data = name;
    this->m_value = name;
}

NodeInfo Identifier::eval(ScopeManager &scope){
    
    return this->info;
}

/* DefStatement Struct */
DefStatement::DefStatement(){
    this->info.type = NodeType::DEF_STM;
    this->m_value = "_DEF";
}

NodeInfo DefStatement::eval(ScopeManager &scope){
    Data *data = scope.findData(std::get<std::string>(m_childrens[0]->eval(scope).data));
    if(data == nullptr){
        std::string identifier = std::get<std::string>(m_childrens[0]->eval(scope).data);
        scope.pushData(identifier, Data(static_cast<void*>(&m_childrens)));
    }

    return this->info;
}

/* RetStatement Struct */
RetStatement::RetStatement(std::shared_ptr<AbstractNode> expression){
    this->info.type = NodeType::RET_STM;
    this->m_value = "_RET";
    attach(expression);
}

NodeInfo RetStatement::eval(ScopeManager &scope){

    return m_childrens[0]->eval(scope);
}

/* FlowPoint Struct */
FlowPoint::FlowPoint(unsigned int flowType){
    switch (flowType){
    case 0:
        this->info.type = NodeType::BRK_STM;
        this->m_value = "_BREAK";
        break;
    case 1:
        this->info.type = NodeType::CON_STM;
        this->m_value = "_CONTINUE";
        break;
    default:
        break;
    }
}

NodeInfo FlowPoint::eval(ScopeManager &scope){

    return this->info;
}

/* CallStatement Struct */
CallStatement::CallStatement(std::string &name, std::shared_ptr<AbstractNode> argsList){
    this->info.type = NodeType::CAL_STM;
    this->m_value = "_CALL";
    attach(std::make_shared<Identifier>(name));
    attach(argsList);
}

NodeInfo CallStatement::eval(ScopeManager &scope){
    std::string identifier = std::get<std::string>(m_childrens[0]->eval(scope).data);
    std::vector<NodeInfo> argsList;
    argsList.reserve(m_childrens[1]->getChildrens().size());

    for(auto &e : m_childrens[1]->getChildrens()){
        argsList.emplace_back(identifierToLiteral(e->eval(scope), scope));
    }
    
    if(Data *data = scope.findData(identifier)){
        std::vector<std::shared_ptr<AbstractNode>> *funDefNodePtr = static_cast<std::vector<std::shared_ptr<AbstractNode>>*>(std::get<void*>(*data));
        std::vector<std::string> paramsList;
        paramsList.reserve(funDefNodePtr->at(1)->getChildrens().size());
        for(auto &e : funDefNodePtr->at(1)->getChildrens()){
            paramsList.emplace_back(std::get<std::string>(e->eval(scope).data));
        }

        if(paramsList.size() == argsList.size()){
            scope.pushScope();
            for(int i = 0; i < paramsList.size(); i++){
                scope.pushData(paramsList[i], argsList[i].data);
            }
            NodeInfo _info = funDefNodePtr->at(2)->eval(scope);
            scope.popScope();
            return _info;
        }else{
            throw ParserException("~Error~ Invalid arguments for \'" + identifier + "\'.");
        }
    }else{
        if(identifier == "print"){
            for(auto &e : argsList){
                std::string toBePrinted = variantAsStr(e.data);
                std::cout << sanitizeStr(toBePrinted) << std::endl;
            }
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

NodeInfo AssignementStatment::eval(ScopeManager &scope){
    std::string identifier = std::get<std::string>(m_childrens[0]->eval(scope).data);
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
NodeInfo identifierToLiteral(NodeInfo info, ScopeManager &scope){
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