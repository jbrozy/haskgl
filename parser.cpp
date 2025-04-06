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

ASTNode *Parser::parse_list() {
  auto root = new ASTNode();
  root->type = NodeType::List;
  root->value = "List";
  consume(TokenType::LeftParen);
  while (current_token.type != TokenType::RightParen) {
    Token identifier = consume(TokenType::Identifier);
    if (current_token.type == TokenType::Comma) {
      printf("parse_list : Comma\n");
      consume(TokenType::Comma);
    }
    auto import = new ASTNode();
    import->type = NodeType::Identifier;
    import->value = identifier.data;
    root->children.emplace_back(import);
  }
  consume(TokenType::RightParen);
  return root;
}

ASTNode *Parser::parse_input() {
  consume(TokenType::Input);
  auto root = new ASTNode{};
  root->type = NodeType::Input;
  auto identifier = consume(TokenType::Identifier);
  consume(TokenType::DoubleColon);
  consume(TokenType::LeftBrace);
  consume(TokenType::NewLine);
  root->value = identifier.data;
  while (current_token.type != TokenType::RightBrace) {
    if (current_token.type == TokenType::Uniforms) {
      consume(TokenType::Uniforms);
      consume(TokenType::DoubleColon);
      consume(TokenType::LeftBrace);
      consume(TokenType::NewLine);
      auto uniforms = new ASTNode{};
      uniforms->type = NodeType::Uniform;
      uniforms->value = "Uniforms";
      while (current_token.type != TokenType::RightBrace) {
        auto sub_type = new ASTNode{};
        sub_type->type = NodeType::Field;
        Token sub_ident = consume(TokenType::Identifier);
        sub_type->value = sub_ident.data;
        consume(TokenType::DoubleColon);
        // complex type
        if (current_token.type == TokenType::Identifier) {
          Token field_type = consume(TokenType::Identifier);
          auto sub = new ASTNode{};
          sub->value = field_type.data;
          sub->type = NodeType::FieldType;
          sub_type->children.emplace_back(sub);
        }
        if (current_token.type == TokenType::Type) {
          Token field_type = consume(TokenType::Type);
          auto sub = new ASTNode{};
          sub->value = field_type.data;
          sub->type = NodeType::FieldType;
          sub_type->children.emplace_back(sub);
        }
        if (current_token.type == TokenType::Comma) {
          consume(TokenType::Comma);
        }
        uniforms->children.emplace_back(sub_type);
        consume(TokenType::NewLine);
      }
      consume(TokenType::RightBrace);
      consume(TokenType::Comma);
      root->children.emplace_back(uniforms);
    }
    if (current_token.type == TokenType::Identifier) {
      Token ident = consume(TokenType::Identifier);
      consume(TokenType::DoubleColon);
      // complex type
      auto field = new ASTNode{};
      field->value = ident.data;
      field->type = NodeType::Field;
      if (current_token.type == TokenType::Type) {
        Token type = consume(TokenType::Type);
        auto field_type = new ASTNode{};
        field_type->value = type.data;
        field_type->type = NodeType::FieldType;
        field->children.emplace_back(field_type);
      }
      // complex type
      if (current_token.type == TokenType::Identifier) {
        Token type = consume(TokenType::Identifier);
        auto field_type = new ASTNode{};
        field_type->value = type.data;
        field_type->type = NodeType::FieldType;
        field->children.emplace_back(field_type);
      }
      root->children.emplace_back(field);
    }
    if (current_token.type == TokenType::Comma) {
      consume(TokenType::Comma);
    }
    consume(TokenType::NewLine);
  }
  consume(TokenType::RightBrace);
  return root;
}

ASTNode *Parser::parse_includes() {
  consume(TokenType::Include);
  auto root = new ASTNode{};
  root->type = NodeType::Include;
  root->value = "Include";
  root->children.emplace_back(parse_list());
  consume(TokenType::RightArrow);
  Token path = consume(TokenType::Identifier);
  auto path_node = new ASTNode{};
  path_node->type = NodeType::Identifier;
  path_node->value = path.data;
  root->children.emplace_back(path_node);
  consume(TokenType::NewLine);
  return root;
}

ASTNode *Parser::parse_primary() {
  switch (current_token.type) {
  case TokenType::Input: {
    consume(TokenType::Input);
    Token ident = consume(TokenType::Identifier);
    return new ASTNode{NodeType::Input, ident.data};
  }
  case TokenType::Output: {
    Token ident = consume(TokenType::Output);
    return new ASTNode{NodeType::Input, ident.data};
  }
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
    if (current_token.type == TokenType::Comma) {
      printf("Comma parse_array\n");
      consume(TokenType::Comma);
    }
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
      if (!aliases->children.empty())
        field->children.emplace_back(aliases);
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
    printf("Parse data def : Comma\n");
    consume(TokenType::Comma);
    consume(TokenType::NewLine);
  }
  consume(TokenType::RightBrace);
  return node;
}

ASTNode *Parser::parse_assignment() {
  fprintf(stdout, "Parsing Assignment.\n");
  Token identifier = consume(TokenType::Identifier);
  fprintf(stdout, "Parsing Equals.\n");
  consume(TokenType::Equals);

  auto value = parse_expression();
  auto assignment = new ASTNode{NodeType::Assignment};
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
    case TokenType::Input: {
      program->children.emplace_back(parse_input());
    } break;
    case TokenType::Include: {
      program->children.emplace_back(parse_includes());
    } break;
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
