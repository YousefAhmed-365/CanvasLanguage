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
            tokens.emplace_back(TokenType::NUM_LIT, e);
        }else if(g_util::isStringLiteral(e)){
            tokens.emplace_back(TokenType::STR_LIT, e);
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

Token *TreeParser::nextToken(){
    if(isEnd()){
        throw ParserException("~Error~ Invalid Token Exception");
    }
    
    return &m_tokens->at(m_currTokenIndex + 1);
}

void TreeParser::consume(std::string tokenStr){
    if(m_currToken->value == tokenStr){
        m_currToken = &m_tokens->at(++m_currTokenIndex);
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
    return nullptr;
}