#include "headers/Token.hpp"

/* Token Struct */
// Variables
// Constructor & Destructor
Token::Token(){
    type = TokenType::NONE;
    value = "";
}

Token::Token(TokenType type, std::string str){
    this->type = type;
    value = str;
}
// Functions
