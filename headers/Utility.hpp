#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <string>
#include <variant>
#include <unordered_map>
#include <unordered_set>

#include "Regex.hpp"
#include "ResManager.hpp"

using Data = std::variant<void*, std::int32_t, float, std::string>;

// Interpreter Utililty
enum class RET_CODE{
    NONE,

    OK,
    ERR
};

enum class OperatorType{
    NONE,

    OPR_ADD,
    OPR_SUB,
    OPR_MUL,
    OPR_DIV,
    OPR_MOD,
    OPR_EXP,
    OPR_INC,
    OPR_INC_DEL,
    OPR_DEC,
    OPR_DEC_DEL,

    LOG_EQL,
    LOG_NEQ,
    LOG_GEQ,
    LOG_GRE,
    LOG_LEQ,
    LOG_LES,
    LOG_AND,
    LOG_LOR,
    LOG_NOT,

    ASG_EQL,
    ASG_ADD,
    ASG_SUB,
    ASG_MUL,
    ASG_DIV,
    ASG_MOD,
    ASG_EXP
};

const std::unordered_map<std::string, OperatorType> operatorStrToken = {
    {"+", OperatorType::OPR_ADD},
    {"-", OperatorType::OPR_SUB},
    {"*", OperatorType::OPR_MUL},
    {"/", OperatorType::OPR_DIV},
    {"%", OperatorType::OPR_MOD},
    {"^", OperatorType::OPR_EXP},
    {"++", OperatorType::OPR_INC},
    {"++_DEL", OperatorType::OPR_INC_DEL},
    {"--", OperatorType::OPR_DEC},
    {"--_DEL", OperatorType::OPR_DEC_DEL},

    {"=", OperatorType::ASG_EQL},
    {"+=", OperatorType::ASG_ADD},
    {"-=", OperatorType::ASG_SUB},
    {"*=", OperatorType::ASG_MUL},
    {"/=", OperatorType::ASG_DIV},
    {"%=", OperatorType::ASG_MOD},
    {"^=", OperatorType::ASG_EXP},

    {"==", OperatorType::LOG_EQL},
    {"!=", OperatorType::LOG_NEQ},
    {">=", OperatorType::LOG_GEQ},
    {">", OperatorType::LOG_GRE},
    {"<=", OperatorType::LOG_LEQ},
    {"<", OperatorType::LOG_LES},
    {"&&", OperatorType::LOG_AND},
    {"||", OperatorType::LOG_LOR},
    {"!", OperatorType::LOG_NOT}
};

const std::unordered_map<OperatorType, std::string> operatorTokenStr = {
    {OperatorType::OPR_ADD, "+"},
    {OperatorType::OPR_SUB, "-"},
    {OperatorType::OPR_MUL, "*"},
    {OperatorType::OPR_DIV, "/"},
    {OperatorType::OPR_MOD, "%"},
    {OperatorType::OPR_EXP, "^"},
    {OperatorType::OPR_INC, "++"},
    {OperatorType::OPR_DEC, "--"},

    {OperatorType::LOG_EQL, "=="},
    {OperatorType::LOG_NEQ, "!="},
    {OperatorType::LOG_GEQ, ">="},
    {OperatorType::LOG_GRE, ">"},
    {OperatorType::LOG_LEQ, "<="},
    {OperatorType::LOG_LES, "<"},
    {OperatorType::LOG_AND, "&&"},
    {OperatorType::LOG_LOR, "||"},
    {OperatorType::LOG_NOT, "!"},
    
    {OperatorType::LOG_NOT, "=="},
    {OperatorType::LOG_NOT, "+="},
    {OperatorType::LOG_NOT, "-="},
    {OperatorType::LOG_NOT, "*="},
    {OperatorType::LOG_NOT, "/="},
    {OperatorType::LOG_NOT, "%="},
    {OperatorType::LOG_NOT, "^="}
};


// Grammar Utility
namespace g_util{
    bool isKeyword(std::string &str);
    bool isOperator(std::string &str);
    bool isSymbol(std::string &str);
    bool isNumLiteral(std::string &str);
    bool isStringLiteral(std::string &str);
    bool isIdentifier(std::string &str);
};

float variantAsNum(Data &data);
std::string variantAsStr(Data &data);
bool isVariantEmptyOrNull(Data data);

std::string stripStr(std::string &str);
std::string sanitizeStr(std::string str);

#endif