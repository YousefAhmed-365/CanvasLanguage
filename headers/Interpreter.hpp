#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "CommonLibs.hpp"

#include "ResManager.hpp"

class Interpreter{
    private:
        // Variables
    public:
        // Variables
        // Constructor & Destructor
        Interpreter();
        ~Interpreter();
        // Functions
        RET_CODE execute(std::string code);
};

#endif