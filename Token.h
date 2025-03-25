#pragma once

#include <string>
#include <string_view>

enum TokenType
{
	Number,
	Comma,
	Identifier,
	LeftParen,
	RightParen,
	LeftBracket,
	RightBracket,
	LeftBrace,
	RightBrace,
	Plus,
	Minus,
	Divide,
	Multiply,
	Equals,
	SemiColon,
	DoubleColon,
	Data,
	End
};

struct Token
{
	TokenType type;
	std::string data;
};
