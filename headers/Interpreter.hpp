#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "CommonLibs.hpp"
#include "ResManager.hpp"
#include "Token.hpp"

class Interpreter{
    private:
        // Variables
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