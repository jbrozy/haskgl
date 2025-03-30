#include "parser.h"
#include "ast_node.h"
#include "token.h"
#include <iostream>
#include <vector>

#define NOT_IMPLEMENTED(str)                                                   \
  do {                                                                         \
    std::cerr << "[NOT IMPLEMENTED] " << __FUNCTION__ << " at " << __FILE__    \
              << ":" << __LINE__ << " : " << str << std::endl;                 \
    std::exit(1);                                                              \
  } while (0)

bool Parser::is_primary(TokenType kind) {
  return kind == TokenType::Identifier || kind == TokenType::LeftParen ||
         kind == TokenType::LeftBracket || kind == TokenType::Number;
}

bool Parser::is_binary(TokenType kind) {
  return kind == TokenType::Plus || kind == TokenType::Minus ||
         kind == TokenType::Multiply || kind == TokenType::Divide;
}

void Parser::advance() { current_token = lexer.next(); }

Token Parser::consume(TokenType expected) {
  if (current_token.type != expected) {
    throw std::runtime_error("Unexpected Token: '" + current_token.data +
                             "', expected: " + std::to_string(expected));
  }

  Token consumed = current_token;
  advance();
  return consumed;
}

ASTNode *Parser::parse_primary() {
  switch (current_token.type) {
  case TokenType::Identifier: {
    Token ident = consume(TokenType::Identifier);
    return new ASTNode{NodeType::Ident, ident.data};
  }
  case TokenType::Number: {
    Token number = consume(TokenType::Number);
    return new ASTNode{NumberLiteral, number.data};
  }
  default:
    break;
  }
}

ASTNode *Parser::parse_expression() {
  ASTNode *left = parse_primary();
  return left;
}

ASTNode *Parser::parse_data_definition() {
  Token data = consume(TokenType::Data);
  ASTNode *node = new ASTNode{};
  Token data_name = consume(TokenType::Identifier);
  node->type = NodeType::TypeDef;
  node->value = data_name.data;

  consume(TokenType::DoubleColon);
  consume(TokenType::LeftBrace);

  consume(TokenType::NewLine);
  if (current_token.type == TokenType::Type) {
    while (current_token.type != TokenType::RightBrace) {
      ASTNode *field = new ASTNode{};
      field->type = NodeType::Field;
      field->value = "Field";

      Token typ = consume(TokenType::Type);
      Token ident = consume(TokenType::Identifier);
      // handle aliases
      if (current_token.type == TokenType::RightArrow) {
        consume(TokenType::RightArrow);
        printf("1\n");
        ASTNode *aliases = new ASTNode{};
        aliases->type = NodeType::AliasList;
        aliases->value = "";
        consume(TokenType::LeftBracket);
        consume(TokenType::RightBracket);
        consume(TokenType::Comma);
        consume(TokenType::NewLine);
        field->children.emplace_back(aliases);
      } else {
        consume(TokenType::Comma);
        consume(TokenType::NewLine);
      }

      ASTNode *field_type = new ASTNode{};
      field_type->type = NodeType::FieldType;
      field_type->value = typ.data;

      ASTNode *field_name = new ASTNode{};
      field_name->type = NodeType::FieldName;
      field_name->value = ident.data;

      field->children.emplace_back(field_type);
      field->children.emplace_back(field_name);

      node->children.emplace_back(field);
    }
  } else if (current_token.type == TokenType::RightBrace) {
    consume(TokenType::RightBrace);
  }

  return node;
}

ASTNode *Parser::parse_assignment() {
  Token identifier = consume(TokenType::Identifier);
  consume(TokenType::Equals);
  ASTNode *value = parse_expression();
  ASTNode *assignment = new ASTNode{NodeType::Assignment};
  assignment->children.push_back(new ASTNode{Ident, identifier.data});
  assignment->children.push_back(value);

  return assignment;
}

ASTNode *Parser::parse() {
  advance();
  ASTNode *program = new ASTNode();
  program->type = NodeType::Program;
  while (current_token.type != TokenType::End) {
    if (current_token.type == TokenType::Identifier) {
      program->children.emplace_back(parse_assignment());
    }
    if (current_token.type == TokenType::Data) {
      program->children.emplace_back(parse_data_definition());
    }
    advance();
  }
  return program;
}
