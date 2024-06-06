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
        std::string m_currTokenStr;
        Token *m_currToken;

        bool m_isParsingUnary;

        // Functions
        bool isEnd();
        bool check(TokenType type);
        bool check(std::string value);
        bool check(TokenType type, std::string value);
        Token *nextToken();
        Token *DelayedConsume(TokenType type);
        void consume();
        void consume(TokenType type);
        void consume(std::string value);
    public:
        // Variables
        // Constructor & Destructor
        TreeParser();
        ~TreeParser();

        // Functions
        std::shared_ptr<AbstractNode> parse(std::vector<Token> &tokenList);

        std::shared_ptr<AbstractNode> parseStatementsList();
        std::shared_ptr<AbstractNode> parseStatement();
        std::shared_ptr<AbstractNode> parseBlockStatement();
        std::shared_ptr<AbstractNode> parseTupleStatement();
        std::shared_ptr<AbstractNode> parseExpression();
        std::shared_ptr<AbstractNode> parseLogicalTerm();
        std::shared_ptr<AbstractNode> parseComparisonTerm();
        std::shared_ptr<AbstractNode> parseTerm();
        std::shared_ptr<AbstractNode> parseExponentialTerm();
        std::shared_ptr<AbstractNode> parseFactor();
        std::shared_ptr<AbstractNode> parseUnary();
};

class Interpreter{
    private:
        // Variables
        TreeParser m_parser;
        ScopeManager m_scopeManager;
    public:
        // Variables
        // Constructor & Destructor
        Interpreter();
        ~Interpreter();

        // Functions
        RET_CODE execute(std::string &str, bool isDebug = false);
        std::vector<Token> lex(const std::string &str, const std::string &pattern);

        void debug_outTokens(std::vector<Token> &tokens);
};

#endif