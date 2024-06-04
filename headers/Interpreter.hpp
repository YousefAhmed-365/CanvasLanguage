#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "AST.hpp"
#include "CommonLibs.hpp"
#include "ResManager.hpp"
#include "Token.hpp"
#include "Error.hpp"

class TreeParser{
    private:
        // Variables
        std::vector<Token> *m_tokens;
        std::uint32_t m_currTokenIndex;
        Token *m_currToken;

        // Functions
        bool isEnd();
        Token *nextToken();
        void consume(std::string tokenStr);
    public:
        // Variables
        // Constructor & Destructor
        TreeParser();
        ~TreeParser();

        // Functions
        std::shared_ptr<AbstractNode> parse(std::vector<Token> &tokenList);

        std::shared_ptr<AbstractNode> parseStatementsList();
};

class Interpreter{
    private:
        // Variables
        TreeParser m_parser;
    public:
        // Variables
        // Constructor & Destructor
        Interpreter();
        ~Interpreter();

        // Functions
        RET_CODE execute(std::string &str);
        std::vector<Token> lex(const std::string &str, const std::string &pattern);

        void debug_outTokens(std::vector<Token> &tokens);
};

#endif