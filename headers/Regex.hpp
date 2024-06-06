#ifndef REGEX_HPP
#define REGEX_HPP

#include <regex>
#include <string>
#include <vector>
#include <boost/regex.hpp>

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
        std::vector<std::string> boostMatchAll(const std::string str) const;
};

std::vector<std::string> PreRegex(const std::string str);

#endif