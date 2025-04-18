#pragma once
#include "token.h"

class Lexer {
public:
  Lexer(const char *lexme) : source(lexme), cursor(0) {}
  Token next();
  Token peek();
  const char *get_sequence() const;
  char peekChar();
  char peekChar(size_t index);
  void skipWhitespace();
  size_t cursor;
  const char *source;

private:
};
