#pragma once
#include "Token.h"

class Lexer
{
public:
	Lexer(const char* lexme) : source(lexme), cursor(0){}
	Token next();
	Token peek();
	char peekChar();
	char peekChar(size_t index);
	void skipWhitespace();
private:
	size_t cursor;
	const char* source;
};

