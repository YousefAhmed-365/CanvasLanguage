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
        str == "ret"    ||
        str == "break"  ||
        str == "continue"    
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
        str == "^=" ||
        str == "^"  ||
        str == "==" ||
        str == ">=" ||
        str == "<=" ||
        str == "!=" ||
        str == "<"  ||
        str == ">"  ||
        str == "!"  ||
        str == "&&" ||
        str == "||" ||
        str == "++" ||
        str == "--"
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
    if(const auto* intPtr = std::get_if<int32_t>(&data)) {
        return *intPtr;
    }else if(const auto* voidPtr = std::get_if<void*>(&data)){
        return reinterpret_cast<long>(voidPtr);
    }
    
    return std::get<float>(data);
}

std::string variantAsStr(Data &data) {
    if(const auto* intPtr = std::get_if<int32_t>(&data)){
        return std::to_string(*intPtr);
    }else if(const auto* floatPtr = std::get_if<float>(&data)){
        return std::to_string(*floatPtr);
    }else if(const auto* voidPtr = std::get_if<void*>(&data)){
        return "(_FUN)" + std::to_string(reinterpret_cast<long>(voidPtr));
    }

    return std::get<std::string>(data);
}

bool isVariantEmptyOrNull(Data data){
    if(const auto *intPtr = std::get_if<int32_t>(&data)){
        return *intPtr == 0;
    }else if(const auto *floatPtr = std::get_if<float>(&data)){
        return *floatPtr == 0;
    }else if(const auto *strPtr = std::get_if<std::string>(&data)){
        return strPtr->empty() || *strPtr == "\"\"";
    }
    
    return true;
}

std::string stripStr(std::string &str){
    return str.length() < 2 ? str : str.substr(1, str.size() - 2);
}

std::string sanitizeStr(std::string str){
    if(str.length() >= 2 && (str[0] == '\"' && str[str.size()-1] == '\"')){
        return str.substr(1, str.size() - 2);
    }

    return str;
}