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
        std::shared_ptr<AbstractNode> parseBlockStatement(bool isPost = false);
        std::shared_ptr<AbstractNode> parseTupleStatement(std::string separator = ",", std::string opening = "(", std::string closing = ")");
        std::shared_ptr<AbstractNode> parseExpression();
        std::shared_ptr<AbstractNode> parseLogicalTerm();
        std::shared_ptr<AbstractNode> parseComparisonTerm();
        std::shared_ptr<AbstractNode> parseTerm();
        std::shared_ptr<AbstractNode> parseExponentialTerm();
        std::shared_ptr<AbstractNode> parseAccessTerm();
        std::shared_ptr<AbstractNode> parseOffsetTerm();
        std::shared_ptr<AbstractNode> parseFactor();
        std::shared_ptr<AbstractNode> parseUnary();
};

enum class DebugType{
    NONE,

    SHOW_PARSING,
    TIME_ONLY,
    DETAILED
};

class Interpreter{
    private:
        // Variables
        TreeParser m_parser;
        ScopeManager m_scopeManager;
        std::shared_ptr<AbstractNode> m_executedRoot;
    public:
        // Variables
        // Constructor & Destructor
        Interpreter();
        ~Interpreter();

        // Functions
        RET_CODE execute(std::string &str, ScopeManager &scope, DebugType debugType = DebugType::NONE);
        std::vector<Token> lex(const std::string &str, const std::string &pattern);

        std::shared_ptr<AbstractNode> getExecutedRoot();
        void debug_outTokens(std::vector<Token> &tokens);
};

#endif