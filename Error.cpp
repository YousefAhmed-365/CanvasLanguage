#include "headers/Error.hpp"

/* Parser Expection Struct */
// Constructor & Destructor
ParserException::ParserException(std::string msg){
    this->m_msg = msg;
}

// Functions
const char* ParserException::what() const noexcept{
    return this->m_msg.c_str();
}