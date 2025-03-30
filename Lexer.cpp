#include "Lexer.h"

#include <ios>
#include <iostream>

Token Lexer::peek() {
  size_t cursor_pos = cursor;
  const Token next_token = next();
  cursor = cursor_pos;
  return next_token;
}

char Lexer::peekChar() { return source[cursor]; }

void Lexer::skipWhitespace() {
  char current = peekChar();
  while (current == ' ') {
    cursor++;
    current = peekChar();
  }
}

char Lexer::peekChar(size_t index) { return source[index]; }

Token Lexer::next() {
  skipWhitespace();

  const char current = peekChar();
  // printf("Current Char: %c\n", current);
  if (current == '\0') {
    return Token{TokenType::End, "<end>"};
  }

  // Handle newlines as tokens
  if (current == '\n') {
    cursor++;
    return Token{TokenType::NewLine, "<newline>"};
  }

  // Handle ::
  if (current == ':') {
    cursor++;
    if (peekChar() == ':') {
      cursor++;
      return Token(TokenType::DoubleColon, "::");
    }
    return Token(TokenType::Colon, ":");
  }

  // Handle ->
  if (current == '-') {
    cursor++;
    if (peekChar() == '>') {
      cursor++;
      return Token(TokenType::RightArrow, "->");
    }
    return Token(TokenType::Minus, "-");
  }

  // Identifiers or keywords
  if (std::isalpha(current)) {
    std::string identifier;
    while (std::isalnum(peekChar()) || peekChar() == '_') {
      identifier += peekChar();
      cursor++;
    }

    // Keywords
    if (identifier == "data")
      return Token{TokenType::Data, identifier};
    if (identifier == "include")
      return Token{TokenType::Include, identifier};
    if (identifier == "uniforms")
      return Token{TokenType::Uniforms, identifier};
    if (identifier == "in")
      return Token{TokenType::Input, identifier};
    if (identifier == "out")
      return Token{TokenType::Output, identifier};
    if (identifier == "float" || identifier == "int")
      return Token{TokenType::Type, identifier};

    return Token{TokenType::Identifier, identifier};
  }

  // Numbers
  if (std::isdigit(current)) {
    std::string number;
    bool dot = false;

    while (std::isdigit(peekChar()) || peekChar() == '.') {
      if (peekChar() == '.') {
        if (dot)
          break;
        dot = true;
      }
      number += peekChar();
      cursor++;
    }
    return Token{TokenType::Number, number};
  }

  // One-character tokens
  cursor++;
  switch (current) {
  case '+':
    return Token{TokenType::Plus, "+"};
  case '*':
    return Token{TokenType::Multiply, "*"};
  case '/':
    return Token{TokenType::Divide, "/"};
  case '{':
    return Token{TokenType::LeftBrace, "{"};
  case '}':
    return Token{TokenType::RightBrace, "}"};
  case '(':
    return Token{TokenType::LeftParen, "("};
  case ')':
    return Token{TokenType::RightParen, ")"};
  case ']':
    return Token{TokenType::RightBracket, "]"};
  case '[':
    return Token{TokenType::LeftBracket, "["};
  case ',':
    return Token{TokenType::Comma, ","};
  case '=':
    return Token{TokenType::Equals, "="};
  case '@':
    return Token{TokenType::At, "@"};
  }
}
