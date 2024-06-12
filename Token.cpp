#include "headers/Token.hpp"

/* Token Struct */
// Variables
// Constructor & Destructor
Token::Token() : type(TokenType::NONE), value(""), col(0), row(0){}

Token::Token(TokenType type, std::string str, unsigned int row, unsigned int col){
    this->type = type;
    this->value = str;
    this->col = col;
    this->row = row;
}
// Functions
