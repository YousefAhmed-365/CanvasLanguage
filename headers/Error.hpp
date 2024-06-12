#ifndef ERROR_HPP
#define ERROR_HPP

#include <exception>
#include <string>

struct Error : public std::exception{
	protected:
		// Variables
		std::string m_msg;
	public:
		// Variables
		// Constructor & Destructor
		Error();
		Error(std::string msg);
		~Error() = default;

		// Functions
		virtual const char* what() const noexcept;
};

struct ParserException : public Error{
	protected:
		// Variables
	public:
		// Variables
		// Constructor & Destructor
		ParserException(std::string msg);
		~ParserException() = default;

		// Functions
		virtual const char* what() const noexcept;
};

struct SyntaxError : public Error{
	protected:
		// Variables
	public:
		// Variables
		// Constructor & Destructor
		SyntaxError(std::string msg);
		SyntaxError(std::string msg, unsigned int row = 0, unsigned int col = 0);
		~SyntaxError() = default;
		
		// Functions
		virtual const char* what() const noexcept;
};

#endif