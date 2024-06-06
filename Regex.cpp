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
    static std::regex regex(m_pattern);

    std::vector<std::string> matches;
    
    auto start = std::sregex_iterator(str.begin(), str.end(), regex);
    auto end = std::sregex_iterator();

    for (auto it = start; it != end; ++it) {
        matches.push_back(it->str());
    }

    return matches;
}

std::vector<std::string> Regex::boostMatchAll(const std::string str) const{
    static const boost::regex pattern(m_pattern);
    
    std::vector<std::string> matches;

    boost::sregex_iterator iter(str.begin(), str.end(), pattern);
    boost::sregex_iterator end;

    for (; iter != end; ++iter) {
        matches.emplace_back(iter->str());
    }

    return matches;
}