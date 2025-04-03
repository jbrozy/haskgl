#pragma once

#include "ast_node.h"
#include "lexer.h"
#include "token.h"

class Parser {
private:
  Lexer &lexer;
  Token current_token = Token{TokenType::None, ""};
  void advance();
  Token consume(TokenType type);
  Token peek_next();
  bool is_primary(TokenType kind);
  bool is_binary(TokenType kind);
  ASTNode *parse_assignment();
  ASTNode *parse_expression();
  ASTNode *parse_function_def();
  ASTNode *parse_data_definition();
  ASTNode *parse_primary();

public:
  Parser(Lexer &lexer) : lexer{lexer} {};
  ASTNode *parse();
};
