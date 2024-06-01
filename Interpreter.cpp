#include "headers/Interpreter.hpp"

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