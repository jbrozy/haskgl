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
        auto aliases = new ASTNode{};
        aliases->type = NodeType::AliasList;
        aliases->value = "";
        consume(TokenType::LeftBracket);
        Token c = current_token;
        bool closed = false;
        while (current_token.type != TokenType::RightBracket) {
          const Token alias = consume(TokenType::Identifier);
          if (current_token.type == TokenType::Comma)
            consume(TokenType::Comma);
          auto alias_node = new ASTNode{};
          alias_node->value = alias.data;
          alias_node->type = NodeType::Alias;
          aliases->children.emplace_back(alias_node);
        }
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
