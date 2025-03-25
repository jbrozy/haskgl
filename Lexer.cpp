#include "Lexer.h"

#include <locale>

Token Lexer::peek()
{
	size_t cursor_pos = cursor;
	const Token next_token = next();
	cursor = cursor_pos;
	return next_token;
}

char Lexer::peekChar()
{
	return source[cursor];
}


void Lexer::skipWhitespace()
{
	char current = peekChar();
	while(current == ' ')
	{
		cursor++;
		current = peekChar();
	}
}

char Lexer::peekChar(size_t index)
{
	return source[index];
}

Token Lexer::next()
{
	skipWhitespace();
	const char current = peekChar();

	if (current == ':')
	{
		cursor++;
		if (peekChar() == ':')
		{
			cursor++;
			return Token(TokenType::DoubleColon, "::");
		}
		// return Token(TokenType::, std::string(1, current));
	}


	if (std::isalpha(current))
	{
		std::string identifier;
		while (std::isalnum(peekChar()) || peekChar() == '_')
		{
			identifier += peekChar();
			cursor++;
		}

		// TODO: check for keywords
		if (identifier == "data"){
			return Token{ Data, identifier };
		}
		else {
			return Token{ Identifier, identifier };
		}
	}

	if(std::isdigit(current))
	{
		std::string number;
		bool dot = false;
		while(std::isalnum(peekChar()) || peekChar() == '.')
		{
			if(peekChar() == '.')
			{
				if (dot) break;
				dot = true;
			}
			number += peekChar();
			cursor++;
		}
		return Token{ Number, number };
	}

	cursor++;
	switch (current)
	{
	case '+': return Token{ TokenType::Plus, "+" };
	case '-': return Token{ TokenType::Minus, "-" };
	case '*': return Token{ TokenType::Multiply, "*" };
	case '/': return Token{ TokenType::Divide, "/" };
	case '=': return Token{ TokenType::Equals, "=" };
	case '\n': return Token{ TokenType::End, "END" };
	}
}


