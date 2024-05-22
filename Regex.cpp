#include "headers/Regex.hpp"
// Varaibles
// Constructor & Destructor
Regex::Regex(std::string pattern) : m_pattern(pattern){}
Regex::~Regex(){}
// Functions
bool Regex::match(const std::string str) const{
    return std::regex_match(str, std::regex(m_pattern));
}
bool Regex::match(const std::string str, std::string pattern){
    return std::regex_match(str, std::regex(pattern));
}
std::vector<std::string> Regex::matchAll(const std::string str) const{
    std::regex regex(m_pattern);
    std::vector<std::string> matches;

    for (auto iter = std::sregex_iterator(str.begin(), str.end(), regex); iter != std::sregex_iterator(); ++iter) {
        matches.emplace_back((*iter).str());
    }

    return matches;
}