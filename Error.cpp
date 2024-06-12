#include "headers/Error.hpp"

/* Error Struct */
// Constructor & Destructor
Error::Error(){
    this->m_msg = "~Error~";
}

Error::Error(std::string msg){
    this->m_msg = msg;
}

// Functions
const char* Error::what() const noexcept{
    return this->m_msg.c_str();
}

/* ParserException Struct */
// Constructor & Destructor
ParserException::ParserException(std::string msg){
    this->m_msg = msg;
}

// Functions
const char* ParserException::what() const noexcept{
    return this->m_msg.c_str();
}

/* SyntaxError Struct */
// Constructor & Destructor
SyntaxError::SyntaxError(std::string msg){
    this->m_msg = "~Syntax Error~ " + msg;
}
SyntaxError::SyntaxError(std::string msg, unsigned int row, unsigned int col){
    this->m_msg = "~Syntax Error " + std::to_string(row) + ':' + std::to_string(col) + "~ " + msg;
}

// Functions
const char* SyntaxError::what() const noexcept{
    return this->m_msg.c_str();
}