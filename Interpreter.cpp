#include "headers/Interpreter.hpp"

/* Interpreter Class */
// Constructor & Destructor
Interpreter::Interpreter(){

}

Interpreter::~Interpreter(){

}

// Functions
RET_CODE Interpreter::execute(std::string &str){
    std::string DEFAULT_REGEX_PATTERN = "(\"[^\"]*\"|[@A-Za-z_]+)|([0-9]+)|(==|>=|>|<=|<|!=|!|&&|\\|\\|)|(\\+\\=|\\-\\=|\\*\\=|\\/\\=|\\=)|(\\+|\\-|\\*|\\/|\\%|\\^|\\.)|(\\(|\\)|\\{|\\}|\\[|\\]|;|:|\\,)";
    std::vector<Token> tokens = lex(str, DEFAULT_REGEX_PATTERN);
    debug_outTokens(tokens);

    try{
        std::shared_ptr<AbstractNode> treeRoot = m_parser.parse(tokens);
        if(treeRoot != nullptr){
            treeRoot->out(0);
        }
    }catch(ParserException err){
        std::cout << err.what() << std::endl;
        
        return RET_CODE::ERR; 
    }

    return RET_CODE::OK;
}

std::vector<Token> Interpreter::lex(const std::string &str, const std::string &pattern){
    const Regex DEFAULT_REGEX(pattern);
    std::vector<std::string> matches = DEFAULT_REGEX.matchAll(str);
    
    std::vector<Token> tokens;
    tokens.reserve(matches.size());

    for(std::string &e : matches){
        if(g_util::isKeyword(e)){
            tokens.emplace_back(TokenType::KEY, e);
        }else if(g_util::isOperator(e)){
            tokens.emplace_back(TokenType::OPR, e);
        }else if(g_util::isSymbol(e)){
            tokens.emplace_back(TokenType::SYM, e);
        }else if(g_util::isNumLiteral(e)){
            tokens.emplace_back(TokenType::LIT, e);
        }else if(g_util::isStringLiteral(e)){
            tokens.emplace_back(TokenType::LIT, e);
        }else if(g_util::isIdentifier(e)){
            tokens.emplace_back(TokenType::IDN, e);
        }
    }

    return tokens;
}

void Interpreter::debug_outTokens(std::vector<Token> &tokens){
    std::map<TokenType, std::string> nameMap = {
        {TokenType::NONE, "NONE"},

        {TokenType::IDN, "IDN"},
        {TokenType::OPR, "OPR"},
        {TokenType::SYM, "SYM"},
        {TokenType::KEY, "KEY"},
        {TokenType::LIT, "LIT"},
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
}

TreeParser::~TreeParser(){

}

// Functions
bool TreeParser::isEnd(){
    return this->m_currTokenIndex == this->m_tokens->size() - 1;
}

bool TreeParser::check(TokenType type){
    return m_currToken->type == type;
}

bool TreeParser::check(std::string value){
    return m_currToken->value == value;
}

bool TreeParser::check(TokenType type, std::string value){
    return m_currToken->type == type && m_currToken->value == value;
}

Token *TreeParser::nextToken(){
    if(isEnd()){
        throw ParserException("~Error~ Invalid Token Exception");
    }
    
    return &m_tokens->at(m_currTokenIndex + 1);
}

void TreeParser::consume(TokenType type){
    if(m_currToken->type == type){
        m_currToken = &m_tokens->at((isEnd()? m_currTokenIndex : ++m_currTokenIndex));
    }else{
        throw ParserException("~Error~ Invalid Token Exception: " + m_currToken->value);
    }
}

void TreeParser::consume(std::string value){
    if(m_currToken->value == value){
        m_currToken = &m_tokens->at((isEnd()? m_currTokenIndex : ++m_currTokenIndex));
    }else{
        throw ParserException("~Error~ Invalid Token Exception: " + m_currToken->value);
    }
}

std::shared_ptr<AbstractNode> TreeParser::parse(std::vector<Token> &tokenList){
    if(tokenList.size() < 1){
        return nullptr;
    }

    m_tokens = &tokenList;
    m_currToken = &m_tokens->at(0);

    return parseStatementsList();
}

std::shared_ptr<AbstractNode> TreeParser::parseStatementsList(){
    std::shared_ptr<AbstractNode> statementsList = std::make_shared<AbstractList>();

    while(!isEnd()){
        if(check("}")){
            consume("}");
            break;
        }
        statementsList->attach(parseStatement());
    }

    return statementsList;
}

std::shared_ptr<AbstractNode> TreeParser::parseStatement(){
    std::shared_ptr<AbstractNode> result;

    if(m_currToken->type == TokenType::LIT || m_currToken->type == TokenType::OPR || check(TokenType::SYM, "(")){
        result = parseExpression();
        consume(";");
    }else if(check(TokenType::SYM, "{")){
        result = parseBlockStatement();
    }else if(check(TokenType::KEY, "if")){
        consume(TokenType::KEY);
        consume("(");
        result = std::make_shared<IfStatement>(parseExpression());
        consume(")");
        result->attach(parseBlockStatement());
    }else{
        throw ParserException("~Error~ Invalid Token Exception: " + m_currToken->value);
    }

    return result;
}

std::shared_ptr<AbstractNode> TreeParser::parseBlockStatement(){
    consume("{");
    std::shared_ptr<AbstractNode> result = parseStatementsList();
    result->setValue("{}");

    return result;
}

std::shared_ptr<AbstractNode> TreeParser::parseExpression(){
    std::shared_ptr<AbstractNode> result = parseLogicalTerm();

    std::string tokenStr;
    while(m_currToken->value == "&&" || m_currToken->value == "||"){
        tokenStr = m_currToken->value;
        consume(TokenType::OPR);
        result = std::make_shared<BinaryExpression>(tokenStr, result, parseLogicalTerm());
    }
    
    return result;
}

std::shared_ptr<AbstractNode> TreeParser::parseLogicalTerm(){
    std::shared_ptr<AbstractNode> result = parseComparisonTerm();

    std::string tokenStr;
    while(m_currToken->value == "==" || m_currToken->value == ">=" || m_currToken->value == ">" || m_currToken->value == "<=" || m_currToken->value == "<" || m_currToken->value == "!="){
        tokenStr = m_currToken->value;
        consume(TokenType::OPR);
        result = std::make_shared<BinaryExpression>(tokenStr, result, parseComparisonTerm());
    }
    
    return result;
}

std::shared_ptr<AbstractNode> TreeParser::parseComparisonTerm(){
    std::shared_ptr<AbstractNode> result = parseTerm();

    std::string tokenStr;
    while(m_currToken->value == "+" || m_currToken->value == "-"){
        tokenStr = m_currToken->value;
        consume(TokenType::OPR);
        result = std::make_shared<BinaryExpression>(tokenStr, result, parseTerm());
    }
    
    return result;
}

std::shared_ptr<AbstractNode> TreeParser::parseTerm(){
    std::shared_ptr<AbstractNode> result = parseExponentialTerm();
    
    std::string tokenStr;
    while(m_currToken->value == "*" || m_currToken->value == "/" || m_currToken->value == "%"){
        tokenStr = m_currToken->value;
        consume(TokenType::OPR);
        result = std::make_shared<BinaryExpression>(tokenStr, result, parseExponentialTerm());
    }
    
    return result;
}

std::shared_ptr<AbstractNode> TreeParser::parseExponentialTerm(){
    std::shared_ptr<AbstractNode> result = parseFactor();

    std::string tokenStr;
    while(m_currToken->value == "^"){
        tokenStr = m_currToken->value;
        consume(TokenType::OPR);
        result = std::make_shared<BinaryExpression>(tokenStr, result, parseFactor());
    }
    
    return result;
}

std::shared_ptr<AbstractNode> TreeParser::parseFactor(){
    std::vector<Token*> unaryOperators;
    std::shared_ptr<AbstractNode> result;

    while (m_currToken->value == "!" || m_currToken->value == "-"){
        unaryOperators.emplace_back(m_currToken);
        consume(TokenType::OPR);
    }

    if(m_currToken->type == TokenType::SYM){
        if(m_currToken->value == "("){
            consume(TokenType::SYM);
            result = parseExpression();
            consume(")");

        }else{
            throw ParserException("~Error~ Invalid Token Exception: " + m_currToken->value);
        }
    }else if(m_currToken->type == TokenType::LIT){
        Data data = m_currToken->value;
        result = std::make_shared<Literal>(data);
        consume(TokenType::LIT);
    }

    for(auto it = unaryOperators.rbegin(); it != unaryOperators.rend(); it++){
        result = std::make_shared<UnaryExpression>((*it)->value, result);
    }

    if(result != nullptr){
        return result;
    }

    throw ParserException("~Error~ Invalid Token Exception: " + m_currToken->value);
}