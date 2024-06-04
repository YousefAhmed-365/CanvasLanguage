#ifndef ERROR_HPP
#define ERROR_HPP

#include <exception>
#include <string>

struct ParserException : public std::exception{
	protected:
		// Variables
		std::string m_msg;
	public:
		// Variables
		// Constructor & Destructor
		ParserException() = default;
		ParserException(std::string msg);
		~ParserException() = default;
		// Functions
		virtual const char* what() const noexcept;
};

#endif