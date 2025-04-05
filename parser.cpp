#include "parser.h"
#include "ast_node.h"
#include "token.h"
#include <iostream>
#include <vector>

constexpr const char *token_to_string(TokenType type) {
  switch (type) {
  case TokenType::Number:
    return "Number";
  case TokenType::At:
    return "At";
  case TokenType::LineBreak:
    return "LineBreak";
  case TokenType::Comma:
    return "Comma";
  case TokenType::Identifier:
    return "Identifier";
  case TokenType::LeftParen:
    return "LeftParen";
  case TokenType::RightParen:
    return "RightParen";
  case TokenType::LeftBracket:
    return "LeftBracket";
  case TokenType::RightBracket:
    return "RightBracket";
  case TokenType::LeftBrace:
    return "LeftBrace";
  case TokenType::RightBrace:
    return "RightBrace";
  case TokenType::Plus:
    return "Plus";
  case TokenType::Minus:
    return "Minus";
  case TokenType::Divide:
    return "Divide";
  case TokenType::Multiply:
    return "Multiply";
  case TokenType::Equals:
    return "Equals";
  case TokenType::SemiColon:
    return "SemiColon";
  case TokenType::DoubleColon:
    return "DoubleColon";
  case TokenType::Data:
    return "Data";
  case TokenType::Input:
    return "Input";
  case TokenType::Output:
    return "Output";
  case TokenType::Include:
    return "Include";
  case TokenType::RightArrow:
    return "RightArrow";
  case TokenType::Uniforms:
    return "Uniforms";
  case TokenType::End:
    return "End";
  case TokenType::None:
    return "None";
  case TokenType::Unknown:
    return "Unknown";
  case TokenType::Colon:
    return "Colon";
  case TokenType::NewLine:
    return "NewLine";
  case TokenType::Type:
    return "Type";
  case TokenType::Tuple:
    return "Tuple";
  case TokenType::List:
    return "List";
  default:
    return "Unknown TokenType";
  }
}

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
                             "', expected: " + token_to_string(expected));
  }

  Token consumed = current_token;
  advance();
  return consumed;
}

ASTNode *Parser::parse_primary() {
  switch (current_token.type) {
  case TokenType::Identifier: {
    Token ident = consume(TokenType::Identifier);
    return new ASTNode{NodeType::Identifier, ident.data};
  }
  case TokenType::Number: {
    Token number = consume(TokenType::Number);
    return new ASTNode{NodeType::NumberLiteral, number.data};
  }
  case TokenType::List: {
    Token list = consume(TokenType::List);
    return new ASTNode{NodeType::List, list.data};
  }
  case TokenType::Tuple: {
    Token tuple = consume(TokenType::Tuple);
    return new ASTNode{NodeType::Tuple, tuple.data};
  }
  default:
    break;
  }
}

ASTNode *Parser::parse_expression() {
  auto base = parse_primary();
  return base;
}

ASTNode *Parser::parse_array() {
  fprintf(stdout, "Parsing Array.\n");
  auto base = new ASTNode{};
  base->type = NodeType::List;
  base->value = "List";
  consume(TokenType::LeftBracket);
  // consume aliases/identifiers
  while (current_token.type != TokenType::RightBracket) {
    Token identifier = consume(TokenType::Identifier);
    auto child = new ASTNode{};
    child->type = NodeType::Identifier;
    child->value = identifier.data;
    if (current_token.type == TokenType::Comma)
      consume(TokenType::Comma);
    base->children.emplace_back(child);
  }
  consume(TokenType::RightBracket);
  return base;
}

ASTNode *Parser::parse_data_definition() {
  fprintf(stdout, "Parsing data definition.\n");
  Token data = consume(TokenType::Data);
  ASTNode *node = new ASTNode{};
  Token identifier = consume(TokenType::Identifier);
  consume(TokenType::DoubleColon);
  consume(TokenType::LeftBrace);
  consume(TokenType::NewLine);
  Token next_token = current_token;
  while (current_token.type != TokenType::RightBrace) {
    Token field_ident = consume(TokenType::Identifier);
    auto field = new ASTNode{};
    field->type = NodeType::Field;
    field->value = field_ident.data;
    if (current_token.type == TokenType::RightArrow) {
      consume(TokenType::RightArrow);
      auto aliases = parse_array();
      aliases->type = NodeType::AliasList;
      if (!aliases->children.empty()) field->children.emplace_back(aliases);
    }
    if (current_token.type == TokenType::DoubleColon) {
      consume(TokenType::DoubleColon);
    }

    Token type_name = consume(TokenType::Type);
    auto type = new ASTNode{};
    type->type = NodeType::Identifier;
    type->value = type_name.data;
    field->children.emplace_back(type);
    node->children.emplace_back(field);
    consume(TokenType::Comma);
    consume(TokenType::NewLine);
  }
  consume(TokenType::RightBrace);
  // // decleration has aliases
  // if (next_token.type == TokenType::RightArrow) {
  //   consume(TokenType::RightArrow);
  //   auto aliases = parse_array();
  //   node->children.emplace_back(aliases);

  //   consume(TokenType::DoubleColon);
  //   Token type = consume(TokenType::Type);
  //   auto type_node = new ASTNode{};
  //   type_node->type = NodeType::FieldType;
  //   type_node->value = type.data;
  //   node->children.emplace_back(type_node);
  //   consume(TokenType::Comma);
  //   consume(TokenType::NewLine);
  // }
  // if (next_token.type == TokenType::DoubleColon) {
  //   consume(TokenType::DoubleColon);
  //   Token type = consume(TokenType::Type);
  //   auto type_node = new ASTNode{};
  //   type_node->type = NodeType::FieldType;
  //   type_node->value = type.data;
  //   node->children.emplace_back(type_node);
  //   consume(TokenType::Comma);
  //   consume(TokenType::NewLine);
  // }
  // consume(TokenType::RightBrace);

  return node;
}

ASTNode *Parser::parse_assignment() {
  fprintf(stdout, "Parsing Assignment.\n");
  Token identifier = consume(TokenType::Identifier);
  consume(TokenType::Equals);

  ASTNode *value = parse_expression();
  ASTNode *assignment = new ASTNode{NodeType::Assignment};
  assignment->children.push_back(
      new ASTNode{NodeType::Identifier, identifier.data});
  assignment->children.push_back(value);

  return assignment;
}

ASTNode *Parser::parse() {
  advance();
  ASTNode *program = new ASTNode();
  program->type = NodeType::Program;
  while (current_token.type != TokenType::End) {
    switch (current_token.type) {
    case TokenType::Identifier: {
      program->children.emplace_back(parse_assignment());
    } break;
    case TokenType::Data: {
      program->children.emplace_back(parse_data_definition());
    } break;
    }
    advance();
  }
  return program;
}
