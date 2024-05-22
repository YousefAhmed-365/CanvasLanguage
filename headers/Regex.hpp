#ifndef REGEX_HPP
#define REGEX_HPP

#include <regex>
#include <string>
#include <vector>

class Regex{
    private:
        //Variables
        std::string m_pattern;
    public:
        //Variables
        //Constructor & Desrtuctor
        Regex(std::string pattern);
        ~Regex();
        //Functions
        bool match(const std::string str) const;
        static bool match(const std::string str, std::string pattern);
        std::vector<std::string> matchAll(const std::string str) const;
};

#endif