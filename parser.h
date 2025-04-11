#pragma once

#include "ast_node.h"
#include "lexer.h"
#include "token.h"
#include <unordered_map>

class Parser {
private:
  Lexer &lexer;
  Token current_token = Token{TokenType::None, ""};
  void advance();
  Token consume(TokenType type);
  Token peek_next();
  bool is_primary(TokenType kind);
  bool is_binary(TokenType kind);
  ASTNode *parse_assignment(const Token &identifier);
  ASTNode *parse_expression(int min_prec);
  ASTNode *parse_function_def(const Token &identifier);
  ASTNode *parse_data_definition();
  ASTNode *parse_array();
  ASTNode *parse_list();
  ASTNode *parse_primary();
  ASTNode *parse_includes();
  ASTNode *parse_input();
  ASTNode *parse_out();
  ASTNode *parse_operator_overload(const Token &token);
  ASTNode *parse_let_in_expr();
  ASTNode *parse_let_binding();
  ASTNode *parse_main_function();
  ASTNode *parse_field_access(ASTNode *base);
  ASTNode *parse_grouped_expression();
  ASTNode *parse_type_signature(const Token &identifier);
  ASTNode *get_function_node(ASTNode *program, std::string function_name);
  int get_precedence(TokenType type) const;
  std::unordered_map<std::string, std::vector<ASTNode *>> pending_signatures;

public:
  Parser(Lexer &lexer) : lexer{lexer} {};
  ASTNode *parse();
};
