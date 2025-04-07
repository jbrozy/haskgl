#pragma once

#include <string>

enum class TokenType {
  Number = 0,
  At = 1,
  LineBreak = 2,
  Comma = 3,
  Identifier = 4,
  LeftParen = 5,
  RightParen = 6,
  LeftBracket = 7,
  RightBracket = 8,
  LeftBrace = 9,
  RightBrace = 10,
  Plus = 11,
  Minus = 12,
  Divide = 13,
  Multiply = 14,
  Equals = 15,
  SemiColon = 16,
  DoubleColon = 17,
  Data = 18,
  Input = 19,
  Output = 20,
  Include = 21,
  RightArrow = 22,
  Uniforms = 23,
  End = 24,
  None = 25,
  Unknown = 26,
  Colon = 27,
  NewLine = 29,
  Type = 30,
  Tuple = 31,
  List = 32,
  Dot = 33,
  Equality = 34,
  Main = 35,
  Let = 36,
  Vertex = 37,
  Fragment = 38,
};

struct Token {
  Token(TokenType type, std::string data) : type(type), data(data) {}
  TokenType type;
  std::string data;
};
