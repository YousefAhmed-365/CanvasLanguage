#include "headers/Interpreter.hpp"

/* Interpreter Class */
// Constructor & Destructor
Interpreter::Interpreter(){

}

Interpreter::~Interpreter(){

}

// Functions
RET_CODE Interpreter::execute(std::string &str, bool isDebug){
    if(str.empty())
        return RET_CODE::OK;

    static const std::string DEFAULT_REGEX_PATTERN = "(\"[^\"]*\"|[@A-Za-z_]+)|([0-9]+)(\\.[0-9]*)?|(==|>=|>|<=|<|!=|!|&&|\\|\\|)|([\\+\\-\\*\\/\\%\\^]?\\=)|(\\+\\+|\\+|\\-\\-|\\-|\\*|\\/|\\%|\\^|\\.)|(\\(|\\)|\\{|\\}|\\[|\\]|;|:|\\,)|(\\n)";
    std::vector<Token> tokens = lex(str, DEFAULT_REGEX_PATTERN);
    
    if(isDebug){
        debug_outTokens(tokens);
    }

    try{
        auto compileStartTime = std::chrono::high_resolution_clock::now();
        std::shared_ptr<AbstractNode> treeRoot = m_parser.parse(tokens);
        auto compileEndTime = std::chrono::high_resolution_clock::now();
        auto compileTime = std::chrono::duration_cast<std::chrono::milliseconds>(compileEndTime - compileStartTime);

        if(treeRoot != nullptr && isDebug){
            std::cout << "\nNode Tree\n->" << std::endl;
            treeRoot->debug_outNodes(0);
        }
        
        NodeInfo rootResult = treeRoot->eval(m_scopeManager);
        auto executionEndTime = std::chrono::high_resolution_clock::now();
        auto executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(executionEndTime - compileStartTime);


        std::cout << "\nExited in " << executionTime.count() << "ms C/E(" << compileTime.count() << "ms, " << executionTime.count() - compileTime.count() << "ms)." << std::endl;
    }catch(Error err){
        std::cout << err.what() << std::endl;

        return RET_CODE::ERR; 
    }

    return RET_CODE::OK;
}

std::vector<Token> Interpreter::lex(const std::string &str, const std::string &pattern){
    Regex DEFAULT_REGEX(pattern);
    std::vector<std::string> matches = DEFAULT_REGEX.boostMatchAll(str);
    
    std::vector<Token> tokens;
    tokens.reserve(matches.size());

    unsigned int row = 0, col = 0;
    for(std::string &e : matches){
        if(e == "\n"){
            ++row;
            col = 0;
        }else if(g_util::isKeyword(e)){
            tokens.emplace_back(TokenType::KEY, e, row, col);
        }else if(g_util::isOperator(e)){
            tokens.emplace_back(TokenType::OPR, e, row, col);
        }else if(g_util::isSymbol(e)){
            tokens.emplace_back(TokenType::SYM, e, row, col);
        }else if(g_util::isNumLiteral(e)){
            tokens.emplace_back(TokenType::NUM_LIT, e, row, col);
        }else if(g_util::isStringLiteral(e)){
            tokens.emplace_back(TokenType::STR_LIT, e, row, col);
        }else if(g_util::isIdentifier(e)){
            tokens.emplace_back(TokenType::IDN, e, row, col);
        }

        ++col;
    }

    return tokens;
}

void Interpreter::debug_outTokens(std::vector<Token> &tokens){
    static std::map<TokenType, std::string> nameMap = {
        {TokenType::NONE, "NONE"},

        {TokenType::IDN, "IDN"},
        {TokenType::OPR, "OPR"},
        {TokenType::SYM, "SYM"},
        {TokenType::KEY, "KEY"},
        {TokenType::LIT, "LIT"},
        {TokenType::NUM_LIT, "NUM_LIT"},
        {TokenType::STR_LIT, "STR_LIT"},
        {TokenType::STR_LIT, "STR"},
        {TokenType::NUM_LIT, "NUM"}
    };

    std::cout << "Tokens\n->\n";
    for(Token &e : tokens){
        std::cout << '[' << nameMap[e.type] << ", \'" << e.value << "\']" << std::endl;
    }
}

/* TreeParser Class */
// Constructor & Destructor
TreeParser::TreeParser(){
    this->m_tokens = nullptr;
    this->m_currToken = nullptr;
    this->m_currTokenIndex = 0;
    this->m_isParsingUnary = false;
}

TreeParser::~TreeParser(){

}

// Functions
bool TreeParser::isEnd(){
    return this->m_currTokenIndex == this->m_tokens->size() - 1;
}

Token *TreeParser::nextToken(){
    if(isEnd()){
        throw SyntaxError("Invalid Token, Reached End of Line. \'" + m_currToken->value + "\'.", m_currToken->row, m_currToken->col);
    }
    
    return &m_tokens->at(m_currTokenIndex + 1);
}

Token *TreeParser::DelayedConsume(TokenType type){
    Token *tokenPtr = m_currToken;
    consume(type);

    return tokenPtr;
}

void TreeParser::consume(){
    if(!isEnd()){
        m_currToken = &m_tokens->at(++m_currTokenIndex);
    }else{
        throw ParserException("~Error~ Reached End of Line.");
    }
}

void TreeParser::consume(TokenType type){
    if(m_currToken->type == type){
        if(!isEnd()){
            m_currToken = &m_tokens->at(++m_currTokenIndex);
        }else{
            throw ParserException("~Error~ Reached End of Line.");
        }
    }else{
        throw SyntaxError("Invalid Token \'" + m_currToken->value + "\'.", m_currToken->row, m_currToken->col);
    }
}

void TreeParser::consume(std::string value){
    if(m_currToken->value == value){
        if(!isEnd()){
            m_currToken = &m_tokens->at(++m_currTokenIndex);
        }else{
            throw ParserException("~Error~ Reached End of Line.");
        }
    }else{
        throw SyntaxError("Invalid Token \'" + m_currToken->value + "\' Expected \'" + value + "\'.", m_currToken->row, m_currToken->col);
    }
}

std::shared_ptr<AbstractNode> TreeParser::parse(std::vector<Token> &tokenList){
    if(tokenList.size() < 1){
        return nullptr;
    }

    m_tokens = &tokenList;
    m_currToken = &m_tokens->at(0);
    
    return parseBlockStatement();
}

std::shared_ptr<AbstractNode> TreeParser::parseBlockStatement(bool isPost){
    consume("{");
    std::shared_ptr<AbstractNode> statementsList;
    
    if(isPost){
        statementsList = std::make_shared<PostBlockStatement>();
    }else{
        statementsList = std::make_shared<BlockStatement>();
    }

    std::shared_ptr<AbstractNode> result;

    while(!isEnd()){
        if(m_currToken->value == "}"){
            consume();
            break;
        }

        result = parseStatement();
        if(result != nullptr){
            statementsList->attach(result);
        }
    }

    return statementsList;
}

std::shared_ptr<AbstractNode> TreeParser::parseStatement(){
    std::shared_ptr<AbstractNode> result;

    if((m_currToken->type == TokenType::NUM_LIT || m_currToken->type == TokenType::STR_LIT) || m_currToken->type == TokenType::IDN || m_currToken->type == TokenType::OPR || m_currToken->value == "("){
        std::string &nextTokenValue = nextToken()->value;
        if(m_currToken->type == TokenType::IDN && (nextTokenValue == "=" || nextTokenValue == "+=" || nextTokenValue == "-=" || nextTokenValue == "*=" 
        || nextTokenValue == "/=" || nextTokenValue == "%=" || nextTokenValue == "^=")){
            std::string identifierStr = m_currToken->value;
            consume();
            std::string oprStr = m_currToken->value;
            consume(TokenType::OPR);
            result = std::make_shared<AssignementStatment>(oprStr, identifierStr, parseExpression());
        }else{
            if(m_currToken->value == "-"){
                m_isParsingUnary = true;
            }
            result = parseExpression();
        }
        
        consume(";");
    }else if(m_currToken->value == "{"){
        result = parseBlockStatement();
    }else if(m_currToken->value == "if"){
        consume(TokenType::KEY);
        consume("(");
        result = std::make_shared<IfStatement>(parseExpression());
        consume(")");
        result->attach(parseBlockStatement(true));

        std::shared_ptr<AbstractNode> alts = std::make_shared<AbstractList>();
        while(m_currToken->value == "elif"){
            consume(TokenType::KEY);
            consume("(");
            alts->attach(std::make_shared<IfStatement>(parseExpression()));
            alts->getChildrens().back()->setValue("_ELIF");
            consume(")");
            alts->getChildrens().back()->attach(parseBlockStatement(true));
        }
        result->attach(alts);

        if(m_currToken->value == "else"){
            consume(TokenType::KEY);
            result->attach(parseBlockStatement());
            result->getChildrens().back()->setValue("_ELSE");
        }
    }else if(m_currToken->value == "while"){
        consume(TokenType::KEY);
        consume("(");
        result = std::make_shared<WhileStatement>(parseExpression());
        consume(")");
        result->attach(parseBlockStatement(true));
    }else if(m_currToken->value == "repeat"){
        consume(TokenType::KEY);
        consume("(");
        result = std::make_shared<RepeatStatement>(parseExpression());
        consume(")");
        result->attach(parseBlockStatement());
    }else if(m_currToken->value == "def"){
        consume(TokenType::KEY);
        result = std::make_shared<DefStatement>();
        result->attach(std::make_shared<Identifier>(DelayedConsume(TokenType::IDN)->value));
        result->attach(parseTupleStatement());
        result->attach(parseBlockStatement(true));
    }else if(m_currToken->value == "ret"){
        consume(TokenType::KEY);
        result = std::make_shared<RetStatement>(parseExpression());
        consume(";");
    }else if(m_currToken->value == "break"){
        consume(TokenType::KEY);
        result = std::make_shared<FlowPoint>(0);
        consume(";");
    }else if(m_currToken->value == "continue"){
        consume(TokenType::KEY);
        result = std::make_shared<FlowPoint>(1);
        consume(";");
    }else{
        if(m_currToken->value == ";"){
            consume(TokenType::SYM);
            return nullptr;
        }

        throw SyntaxError("Invalid Token \'" + m_currToken->value + "\'.", m_currToken->row, m_currToken->col);
    }

    return result;
}

std::shared_ptr<AbstractNode> TreeParser::parseTupleStatement(){
    std::shared_ptr<AbstractList> result = std::make_shared<AbstractList>();
    
    consume("(");
    while(m_currToken->value != ")"){
        result->attach(parseExpression());

        if(m_currToken->value == "," && nextToken()->value != ")"){
            consume();
        }else{
            if(m_currToken->value != ")"){
                throw SyntaxError("Invalid Token \'" + m_currToken->value + "\'.", m_currToken->row, m_currToken->col);
            }
        }
    }
    consume(")");

    return result;
}

std::shared_ptr<AbstractNode> TreeParser::parseExpression(){
    std::shared_ptr<AbstractNode> result = parseLogicalTerm();

    std::string tokenStr;
    while(m_currToken->value == "&&" || m_currToken->value == "||"){
        tokenStr = DelayedConsume(TokenType::OPR)->value;
        result = std::make_shared<BinaryExpression>(tokenStr, result, parseLogicalTerm());
    }
    
    return result;
}

std::shared_ptr<AbstractNode> TreeParser::parseLogicalTerm(){
    std::shared_ptr<AbstractNode> result = parseComparisonTerm();

    std::string tokenStr;
    while(m_currToken->value == "==" || m_currToken->value == ">=" || m_currToken->value == ">" || m_currToken->value == "<=" || m_currToken->value == "<" || m_currToken->value == "!="){
        tokenStr = DelayedConsume(TokenType::OPR)->value;
        result = std::make_shared<BinaryExpression>(tokenStr, result, parseComparisonTerm());
    }
    
    return result;
}

std::shared_ptr<AbstractNode> TreeParser::parseComparisonTerm(){
    std::shared_ptr<AbstractNode> result = parseTerm();

    std::string tokenStr;
    while(m_currToken->value == "+" || (!m_isParsingUnary && m_currToken->value == "-")){
        tokenStr = DelayedConsume(TokenType::OPR)->value;
        result = std::make_shared<BinaryExpression>(tokenStr, result, parseTerm());
    }
    
    return result;
}

std::shared_ptr<AbstractNode> TreeParser::parseTerm(){
    std::shared_ptr<AbstractNode> result = parseExponentialTerm();
    
    std::string tokenStr;
    while(m_currToken->value == "*" || m_currToken->value == "/" || m_currToken->value == "%"){
        tokenStr = m_currToken->value;
        consume();
        result = std::make_shared<BinaryExpression>(tokenStr, result, parseExponentialTerm());
    }
    
    return result;
}

std::shared_ptr<AbstractNode> TreeParser::parseExponentialTerm(){
    std::shared_ptr<AbstractNode> result = parseFactor();

    std::string tokenStr;
    while(m_currToken->value == "^"){
        tokenStr = DelayedConsume(TokenType::OPR)->value;
        result = std::make_shared<BinaryExpression>(tokenStr, result, parseFactor());
    }
    
    return result;
}

std::shared_ptr<AbstractNode> TreeParser::parseFactor(){
    std::vector<Token*> unaryOperators;
    std::shared_ptr<AbstractNode> result;

    while (m_currToken->value == "!" || m_currToken->value == "-"){
        unaryOperators.emplace_back(m_currToken);
        consume();
    }

    switch (m_currToken->type){
    case TokenType::OPR:
        if((m_currToken->value == "++" || m_currToken->value == "--") && nextToken()->type == TokenType::IDN){
            std::string &oprStr = DelayedConsume(TokenType::OPR)->value;
            std::string &identifier = DelayedConsume(TokenType::IDN)->value;
            result = std::make_shared<UnaryExpression>(oprStr, std::make_shared<Identifier>(identifier));
        }
        break;
    case TokenType::SYM:
        if(m_currToken->value == "("){
            consume();
            result = parseExpression();
            consume(")");
        }
        break;
    case TokenType::NUM_LIT:
        {
            Data data = m_currToken->value;
            data = std::stof(std::get<std::string>(data));
            result = std::make_shared<Literal>(data);
            result->info.type = NodeType::NUM_LIT;
            consume();
        }
        break;
    case TokenType::STR_LIT:
        {
            Data data = m_currToken->value;
            result = std::make_shared<Literal>(data);
            result->info.type = NodeType::STR_LIT;
            consume();
        }
        break;
    case TokenType::IDN:
        {
        std::string &nextTokenValue = nextToken()->value;
        if(nextTokenValue == "("){
            std::string &identifier = DelayedConsume(TokenType::IDN)->value;
            result = std::make_shared<CallStatement>(identifier, parseTupleStatement());
        }else if(nextTokenValue == "++" || nextTokenValue == "--"){
            std::string &identifier = DelayedConsume(TokenType::IDN)->value;
            std::string oprStr = DelayedConsume(TokenType::OPR)->value + "_DEL";
            result = std::make_shared<UnaryExpression>(oprStr, std::make_shared<Identifier>(identifier));
        }else{
            result = std::make_shared<Identifier>(m_currToken->value);
            consume();
        }
        }
        break;
    
    default:
        break;
    }

    if(m_currToken->value == "."){
        std::string &oprStr = DelayedConsume(TokenType::OPR)->value;
        std::shared_ptr<AbstractNode> rightOperand = parseExpression();
        
        if(rightOperand->getValue() == "."){
            throw SyntaxError("Can't nest access operators.", m_currToken->row, m_currToken->col);
        }

        result = std::make_shared<BinaryExpression>(oprStr, result, rightOperand);
    }

    for(auto it = unaryOperators.rbegin(); it != unaryOperators.rend(); ++it){
        result = std::make_shared<UnaryExpression>((*it)->value, result);
    }

    if(result != nullptr){
        return result;
    }

    throw SyntaxError("Invalid Token \'" + m_currToken->value + "\'.", m_currToken->row, m_currToken->col);
}