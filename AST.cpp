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
    this->m_value = "_ABS[]";
}

NodeInfo AbstractList::eval(ScopeManager &scope){
    this->info.data = this;
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
        NodeInfo _info = e->eval(scope); 

        if(scope.isReturning){
            scope.popScope();
            return _info;
        }
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

        if(_info.type == NodeType::BRK_STM || _info.type == NodeType::CON_STM || scope.isReturning){
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
        if(scope.isReturning){
            scope.popScope();
            return _info;
        }

        if(_info.type == NodeType::BRK_STM){
            scope.popScope();
            break;
        }else if(_info.type == NodeType::CON_STM){
            continue;
        }
    }
    scope.popScope();
    
    return this->info;
}

/* ForStatement Struct */
ForStatement::ForStatement(){
    this->info.type = NodeType::WHL_STM;
    this->m_value = "_FOR";
}

NodeInfo ForStatement::eval(ScopeManager &scope){
    if(!m_childrens[0]->getChildrens().empty()){
        scope.pushScope();
        NodeInfo firstStatement = m_childrens[0]->getChildrens().at(0)->eval(scope);
        NodeInfo secondStatement = m_childrens[0]->getChildrens().at(1)->eval(scope);
        NodeInfo thirdStatement;
        while(!isVariantEmptyOrNull(identifierToLiteral(secondStatement, scope).data)){
            NodeInfo _info = m_childrens[1]->eval(scope);
            if(scope.isReturning){
                scope.popScope();
                return _info;
            }

            if(_info.type == NodeType::BRK_STM){
                scope.popScope();
                break;
            }else if(_info.type == NodeType::CON_STM){
                continue;
            }

            thirdStatement = m_childrens[0]->getChildrens().at(2)->eval(scope);
            secondStatement = m_childrens[0]->getChildrens().at(1)->eval(scope);
        }
        scope.popScope();
    }
    
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
        scope.pushScope();
        unsigned int count = variantAsNum(expression.data);
        NodeInfo _info;
        for(unsigned int i = 0; i < count; ++i){
            NodeInfo _info = m_childrens[1]->eval(scope);
            if(scope.isReturning){
                scope.popScope();
                return _info;
            }
            
            if(_info.type == NodeType::BRK_STM){
                scope.popScope();
                break;
            }else if(_info.type == NodeType::CON_STM){
                continue;
            }
        }
        scope.popScope();
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
                leftNode.type = NodeType::NUM_LIT;
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
        case OperatorType::OPR_OFF:
            if(rightNode.type == NodeType::NUM_LIT){
                int index = static_cast<int>(variantAsNum(rightNode.data));
                if(leftNode.type == NodeType::ABS_LST){
                    AbstractList* _data = static_cast<AbstractList*>(std::get<void*>(leftNode.data));
                    if(index < _data->getChildrens().size() && index >= 0){
                        leftNode = _data->getChildrens()[static_cast<int>(variantAsNum(rightNode.data))]->eval(scope);
                    }else{
                        throw ParserException("~Error~ Out of bounds exception");
                    }
                }else if(leftNode.type == NodeType::STR_LIT){
                    std::string _str = stripStr(std::get<std::string>(leftNode.data)); 
                    if(index < _str.size() && index >= 0){
                        leftNode.data =  '\"' + std::string(1, _str[index]) + '\"';
                    }else{
                        throw ParserException("~Error~ Out of bounds exception");
                    }
                }
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

    std::string identifier;
    Data *data = nullptr;
    if(leftNode.type == NodeType::IDN){
        identifier = std::get<std::string>(leftNode.data);
        data = scope.findData(identifier);
        leftNode = identifierToLiteral(leftNode, scope);
    }

    switch (this->type){
    case OperatorType::LOG_NOT:
        if(leftNode.type == NodeType::NUM_LIT){
                leftNode.data = !variantAsNum(leftNode.data);
        }else{
            throw ParserException("~Error~ Invalid Unary Operation \'" + m_value + variantAsStr(leftNode.data) + "\' Incompatible Type.");
        }
        break;

    case OperatorType::OPR_SUB:
        if(leftNode.type == NodeType::NUM_LIT){
            leftNode.data = -variantAsNum(leftNode.data);
        }else{
            throw ParserException("~Error~ Invalid Unary Operation \'" + m_value + variantAsStr(leftNode.data) + "\' Incompatible Type.");
        }
        break;

    case OperatorType::OPR_INC:
        if(data != nullptr && leftNode.type == NodeType::NUM_LIT){
            leftNode.data = variantAsNum(leftNode.data) + 1;
            *data = leftNode.data;
        }else{
            throw ParserException("~Error~ Invalid Unary Operation \'" + m_value + variantAsStr(leftNode.data) + "\' Incompatible Type.");
        }
        break;
    case OperatorType::OPR_INC_DEL:
        if(data != nullptr && leftNode.type == NodeType::NUM_LIT){
            *data = variantAsNum(leftNode.data) + 1;
        }else{
            throw ParserException("~Error~ Invalid Unary Operation \'" + m_value + variantAsStr(leftNode.data) + "\' Incompatible Type.");
        }
        break;
    case OperatorType::OPR_DEC:
        if(data != nullptr && leftNode.type == NodeType::NUM_LIT){
            leftNode.data = variantAsNum(leftNode.data) - 1;
            *data = leftNode.data;
        }else{
            throw ParserException("~Error~ Invalid Unary Operation \'" + m_value + variantAsStr(leftNode.data) + "\' Incompatible Type.");
        }
        break;
    case OperatorType::OPR_DEC_DEL:
        if(data != nullptr && leftNode.type == NodeType::NUM_LIT){
            *data = variantAsNum(leftNode.data) - 1;
        }else{
            throw ParserException("~Error~ Invalid Unary Operation \'" + m_value + variantAsStr(leftNode.data) + "\' Incompatible Type.");
        }
        break;
    default:
        break;
    }

    return leftNode;
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
    NodeInfo _info = identifierToLiteral(m_childrens[0]->eval(scope), scope); 
    scope.isReturning = true;
    return _info;
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
            scope.isReturning = false;
            scope.popScope();
            return _info;
        }else{
            throw ParserException("~Error~ Invalid arguments for \'" + identifier + "\'.");
        }
    }else{
        // Handling Predefined Functions.
        if(identifier == "print"){
            for (auto &e : argsList) {
                std::string _str = sanitizeStr(variantAsStr(e.data));
                for (size_t i = 0; i < _str.length(); ++i) {
                    if(_str[i] == '\\' && i + 1 < _str.length()) {
                        if (_str[i + 1] == 'n') {
                            std::cout << '\n';
                            ++i;
                        } else if (_str[i + 1] == '\\') {
                            std::cout << '\\';
                            ++i;
                        } else {
                            std::cout << '\\' << _str[i + 1];
                            ++i;
                        }
                    }else{
                        std::cout << _str[i];
                    }
                }
            } std::cout << std::endl;
        }else if(identifier == "printf"){
            if(!argsList.empty()){
                std::string _formatStr = sanitizeStr(variantAsStr(argsList[0].data));

                size_t argIndex = 1;
                for(size_t i = 0; i < _formatStr.length(); ++i){
                    if(_formatStr[i] == '%' && i + 1 < _formatStr.length() && _formatStr[i + 1] == 's'){
                        if(argIndex < argsList.size()){
                            std::cout << sanitizeStr(variantAsStr(argsList[argIndex].data));
                            ++argIndex;
                        }else{
                            std::cout << "%s";
                        }
                        ++i;
                    }else{
                        if(_formatStr[i] == '\\' && i + 1 < _formatStr.length()) {
                            if (_formatStr[i + 1] == 'n') {
                                std::cout << '\n';
                                ++i;
                            } else if (_formatStr[i + 1] == '\\') {
                                std::cout << '\\';
                                ++i;
                            } else {
                                std::cout << '\\' << _formatStr[i + 1];
                                ++i;
                            }
                        }else{
                            std::cout << _formatStr[i];
                        }
                    }
                }
            }else{
                throw ParserException("~Error~ Invalid arguments for \'" + identifier + "\'.");
            }
        }else if(identifier == "input"){
            for(auto &e : argsList){
                std::string toBePrinted = variantAsStr(e.data);
                std::cout << sanitizeStr(toBePrinted);
            }
            std::string inputStr;
            std::getline(std::cin, inputStr);

            return NodeInfo(NodeType::STR_LIT, '\"' + inputStr + '\"');
        }else if(identifier == "to_num"){
            if(argsList.size() == 1){
                if(argsList[0].type == NodeType::NUM_LIT){
                    return argsList[0];
                }else if(argsList[0].type == NodeType::STR_LIT){
                    std::string _strContent = stripStr(std::get<std::string>(argsList[0].data));
                    if(g_util::isNumLiteral(_strContent)){
                        return NodeInfo(NodeType::NUM_LIT, std::stof(_strContent));
                    }
                }
            }else{
                throw ParserException("~Error~ Invalid arguments for \'" + identifier + "\'.");
            }
        }else if(identifier == "to_str"){
            if(argsList.size() == 1){
                if(argsList[0].type == NodeType::STR_LIT){
                    return argsList[0];
                }else if(argsList[0].type == NodeType::NUM_LIT){
                    return NodeInfo(NodeType::STR_LIT, '\"' + variantAsStr(argsList[0].data) + '\"');
                }
            }else{
                throw ParserException("~Error~ Invalid arguments for \'" + identifier + "\'.");
            }
        }else if(identifier == "error"){
            if(argsList.size() == 1){
                throw Error(variantAsStr(argsList[0].data));
            }else{
                throw ParserException("~Error~ Invalid arguments for \'" + identifier + "\'.");
            }
        }else if(identifier == "import"){
            if(argsList.size() == 2){
                if(argsList[0].type == NodeType::STR_LIT && argsList[1].type == NodeType::STR_LIT){
                    if(stripStr(std::get<std::string>(argsList[0].data)) == "LIB"){
                        std::string importName = stripStr(std::get<std::string>(argsList[1].data));
                        for(auto &e : scope.globalImportStack){
                            if(importName == e){
                                throw ParserException("~Error~ Recursive imports \'" + importName + "\'.");
                            }
                        }
                        scope.globalImportStack.emplace_back(importName);

                        Interpreter libInterpreter;
                        std::string code = loadFileContentAsCode(importName);
                        libInterpreter.execute(code, scope, true);
                        std::shared_ptr<AbstractNode> treeRoot = libInterpreter.getExecutedRoot();
                        scope.globalImportStack.pop_back();
                    }else{
                        throw ParserException("~Error~ Invalid import type for \'" + stripStr(std::get<std::string>(argsList[1].data)) + "\'.");
                    }
                }else{
                    throw ParserException("~Error~ Invalid arguments for \'" + identifier + "\'.");
                }
            }else{
                throw ParserException("~Error~ Invalid arguments for \'" + identifier + "\'.");
            }
        }else{
            throw ParserException("~Error~ Undefined Identifier \'" + identifier + "\'");
        }
    }

    return this->info;
}

/* AssignementStatment Struct */
AssignementStatment::AssignementStatment(std::string &oprStr, std::string &identifier, std::shared_ptr<AbstractNode> expression){
    this->info.type = NodeType::ASG_STM;
    this->type = operatorStrToken.at(oprStr);
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
        switch (this->type){
        case OperatorType::ASG_EQL:
            *data = expression.data;
            break;
        case OperatorType::ASG_ADD:
            if(expression.type == NodeType::NUM_LIT && (std::holds_alternative<float>(*data) || std::holds_alternative<int32_t>(*data))){
                *data = variantAsNum(*data) + variantAsNum(expression.data);
            }else if(expression.type == NodeType::STR_LIT && (std::holds_alternative<std::string>(*data))){
                *data = '\"' + stripStr(std::get<std::string>(*data)) + stripStr(std::get<std::string>(expression.data)) + '\"';
            }else{
                throw ParserException("~Error~ Invalid Assignement \'" + this->m_value + "\' for Identifier \'" + identifier + "\'.");
            }
            break;
        case OperatorType::ASG_SUB:
            if(expression.type == NodeType::NUM_LIT && (std::holds_alternative<float>(*data) || std::holds_alternative<int32_t>(*data))){
                *data = variantAsNum(*data) - variantAsNum(expression.data);
            }else{
                throw ParserException("~Error~ Invalid Assignement \'" + this->m_value + "\' for Identifier \'" + identifier + "\'.");
            }
            break;
        case OperatorType::ASG_MUL:
            if(expression.type == NodeType::NUM_LIT && (std::holds_alternative<float>(*data) || std::holds_alternative<int32_t>(*data))){
                *data = variantAsNum(*data) * variantAsNum(expression.data);
            }else{
                throw ParserException("~Error~ Invalid Assignement \'" + this->m_value + "\' for Identifier \'" + identifier + "\'.");
            }
            break;
        case OperatorType::ASG_DIV:
            if(expression.type == NodeType::NUM_LIT && (std::holds_alternative<float>(*data) || std::holds_alternative<int32_t>(*data))){
                *data = variantAsNum(*data) / variantAsNum(expression.data);
            }else{
                throw ParserException("~Error~ Invalid Assignement \'" + this->m_value + "\' for Identifier \'" + identifier + "\'.");
            }
            break;
        case OperatorType::ASG_MOD:
            if(expression.type == NodeType::NUM_LIT && (std::holds_alternative<float>(*data) || std::holds_alternative<int32_t>(*data))){
                *data = static_cast<int>(variantAsNum(*data)) % static_cast<int>(variantAsNum(expression.data));
            }else{
                throw ParserException("~Error~ Invalid Assignement \'" + this->m_value + "\' for Identifier \'" + identifier + "\'.");
            }
            break;
        case OperatorType::ASG_EXP:
            if(expression.type == NodeType::NUM_LIT && (std::holds_alternative<float>(*data) || std::holds_alternative<int32_t>(*data))){
                *data = std::pow(variantAsNum(*data), variantAsNum(expression.data));
            }else{
                throw ParserException("~Error~ Invalid Assignement \'" + this->m_value + "\' for Identifier \'" + identifier + "\'.");
            }
            break;
        
        default:
            break;
        }
    }
    
    return expression;
}

// Helper Functions
NodeInfo identifierToLiteral(NodeInfo info, ScopeManager &scope){
    if(std::holds_alternative<void*>(info.data)){

    }

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

    if(std::holds_alternative<int32_t>(*data) || std::holds_alternative<float>(*data) || std::holds_alternative<void*>(*data)){
        return NodeInfo(NodeType::NUM_LIT, *data);
    }

    return NodeInfo(NodeType::STR_LIT, *data);
}