#include "headers/Utility.hpp"

// Grammar Utility
bool g_util::isKeyword(std::string &str){
    return (
        str == "if"     ||
        str == "elif"   ||
        str == "else"   ||
        str == "while"  ||
        str == "repeat" ||
        str == "def"    ||
        str == "ret"    
    );
}

bool g_util::isOperator(std::string &str){
    return (
        str == "+"  ||
        str == "-"  ||
        str == "*"  ||
        str == "/"  ||
        str == "%"  ||
        str == "="  ||
        str == "+=" ||
        str == "-=" ||
        str == "*=" ||    
        str == "/=" ||
        str == "%=" ||
        str == "^"  ||
        str == "==" ||
        str == ">=" ||
        str == "<=" ||
        str == "!=" ||
        str == "<"  ||
        str == ">"  ||
        str == "!"  ||
        str == "&&" ||
        str == "||"
    );
}

bool g_util::isSymbol(std::string &str){
    return (
        str == "{"  ||
        str == "}"  ||
        str == "("  ||
        str == ")"  ||
        str == "["  ||
        str == "]"  ||
        str == ":"  ||
        str == ";"  ||
        str == "," 
    );
}

bool g_util::isNumLiteral(std::string &str){
    bool isFloat = false;
    for(size_t i = 0; i < str.length(); i++){
        if(!std::isdigit(str[i])){
            if(str[i] == '.' && !isFloat){
                isFloat = true;
                continue;
            }
            return false;
        }
    }

    return true;
}

bool g_util::isStringLiteral(std::string &str){
    return str.length() < 2? false : (str[0] == '\"' && str[str.length()-1] == '\"');
}

bool g_util::isIdentifier(std::string &str){
    for(auto &e : str){
        if(!std::isalpha(e) && e != '@' && e != '_'){
            return false;
        }
    }

    return !isKeyword(str);
}

float variantAsNum(Data &data){
    if(std::holds_alternative<int32_t>(data)){
        return std::get<int32_t>(data);
    }

    return std::get<float>(data);
}