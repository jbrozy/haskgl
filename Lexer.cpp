#include "Lexer.h"

#include "ast_node.h"
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

const char *Lexer::get_sequence() const {
  char *slice = (char *)malloc(sizeof(char) * (cursor + 1));
  size_t i = 0;
  for (i = 0; i < cursor; ++i) {
    slice[i] = source[i];
  }
  slice[i + 1] = '\0';
  return slice;
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
  if (std::isalpha(current) || current == '@') {
    std::string identifier;
    if (peekChar() == '@') {
      identifier += peekChar();
      cursor++;
    }
    while (std::isalnum(peekChar()) || peekChar() == '_') {
      identifier += peekChar();
      cursor++;
    }

    // Keywords
    fprintf(stdout, "Identifier: %s\n", identifier.c_str());
    if (identifier == "vertex") {
      return Token{TokenType::Identifier, identifier};
    }
    if (identifier == "fragment") {
      return Token{TokenType::Identifier, identifier};
    }
    if (identifier == "let") {
      return Token{TokenType::Let, identifier};
    }
    if (identifier == "data") {
      return Token{TokenType::Data, identifier};
    }
    if (identifier == "@internal") {
      return Token{TokenType::Internal, identifier};
    }
    if (identifier == "@main") {
      return Token{TokenType::Main, identifier};
    }
    if (identifier == "@include") {
      return Token{TokenType::Include, identifier};
    }
    if (identifier == "@uniform") {
      return Token{TokenType::Uniforms, identifier};
    }
    if (identifier == "@in") {
      return Token{TokenType::Input, identifier};
    }
    if (identifier == "@out") {
      return Token{TokenType::Output, identifier};
    }
    // types
    // TODO: add different types
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
  case '>': {
    if (peekChar(cursor + 1) == '=') {
      cursor++;
      return Token{TokenType::GreaterThan, "/"};
    }
    return Token{TokenType::GreaterThan, "/"};
  }
  case '<': {
    if (peekChar(cursor + 1) == '=') {
      cursor++;
      return Token{TokenType::LessThan, "/"};
    }
    return Token{TokenType::LessThan, "/"};
  }
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
  case '.':
    return Token{TokenType::Dot, "."};
  }
}
