#include "parser.h"
#include "ast_node.h"
#include "token.h"
#include <iostream>
#include <vector>

int Parser::get_precedence(TokenType type) const {
  static std::unordered_map<TokenType, int> prec = {
      {TokenType::Equality, 10},
      {TokenType::Plus, 20},
      {TokenType::Minus, 20},
      {TokenType::Multiply, 30},
      {TokenType::Divide, 30},
      {TokenType::Dot, 100}, // Highest precedence for member access
  };

  auto it = prec.find(type);
  return it != prec.end() ? it->second : -1;
}

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
  case TokenType::Internal:
    return "Internal";
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

ASTNode *Parser::parse_let_binding() {
  Token name = consume(TokenType::Identifier);
  consume(TokenType::Equals);
  ASTNode *expr = parse_expression(0);

  auto binding = new ASTNode{NodeType::LetBinding, name.data};
  binding->children.push_back(expr);
  return binding;
}

ASTNode *Parser::parse_let_in_expr() {
  auto root = new ASTNode{};
  root->type = NodeType::LetInExpr;
  consume(TokenType::Let);
  Token identifier = consume(TokenType::Identifier);
  root->value = identifier.data;
  consume(TokenType::Equals);

  auto binding = new ASTNode{NodeType::LetBinding, identifier.data};
  binding->children.push_back(parse_expression(0));
  consume(TokenType::NewLine);

  root->children.push_back(binding);

  while (current_token.type != TokenType::Input) {
    if (current_token.type == TokenType::NewLine)
      consume(TokenType::NewLine);
    Token identifier = consume(TokenType::Identifier);
    consume(TokenType::Equals);
    auto expr = parse_expression(0);
    auto binding = new ASTNode{NodeType::LetBinding, identifier.data};
    binding->children.push_back(expr);
    root->children.emplace_back(binding);
    if (current_token.type == TokenType::NewLine)
      consume(TokenType::NewLine);
  }

  auto in = new ASTNode{};
  in->type = NodeType::Input;
  consume(TokenType::Input);
  Token a = consume(TokenType::Identifier);
  Token b = consume(TokenType::Identifier);
  in->children.push_back(new ASTNode{NodeType::Identifier, a.data});
  in->children.push_back(new ASTNode{NodeType::Identifier, b.data});
  root->children.push_back(in);

  return root;
}

ASTNode *Parser::parse_main_function() {
  auto root = new ASTNode{};
  consume(TokenType::Main);
  root->type = NodeType::EntryPoint;
  Token t = consume(TokenType::Identifier);
  root->value = t.data;
  consume(TokenType::Equals);
  if (current_token.type == TokenType::NewLine)
    consume(TokenType::NewLine);
  if (current_token.type == TokenType::Let) {
    root->children.emplace_back(parse_let_in_expr());
  }

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

ASTNode *Parser::parse_field_access(ASTNode *base) {
  while (current_token.type == TokenType::Dot) {
    consume(TokenType::Dot);
    Token field = consume(TokenType::Identifier);

    ASTNode *access = new ASTNode{NodeType::FieldAccess, field.data};
    access->children.push_back(base);
    base = access;
  }
  return base;
}

ASTNode *Parser::parse_grouped_expression() {
  consume(TokenType::LeftParen);
  auto expr = parse_expression(0);
  consume(TokenType::RightParen);
  return parse_field_access(expr);
}

ASTNode *Parser::parse_primary() {
  ASTNode *node = new ASTNode();
  if (current_token.type == TokenType::Number) {
    node->type = NodeType::NumberLiteral;
    node->value = current_token.data;
    consume(TokenType::Number);

    return node;
  }
  if (current_token.type == TokenType::Type) {
    node->type = NodeType::Identifier;
    node->value = current_token.data;
    consume(TokenType::Identifier);

    return node;
  }
  if (current_token.type == TokenType::LeftParen) {
    return parse_grouped_expression();
  }
  if (current_token.type == TokenType::Identifier) {
    ASTNode *node = new ASTNode();
    node->type = NodeType::Identifier;
    node->value = current_token.data;
    consume(TokenType::Identifier);

    return node;
  }

  // Handle function application chaining
  while (is_primary(current_token.type)) {
    ASTNode *arg = parse_primary();
    ASTNode *call = new ASTNode{NodeType::FunctionApplication};
    call->children.push_back(node); // function
    call->children.push_back(arg);  // argument
    node = call;
  }
  if (node != nullptr) {
    return parse_field_access(node);
  }

  // Add support for literals, parentheses, etc. here
  std::cerr << "Unexpected token in primary expression: " << current_token.data
            << "\n";
  return nullptr;
}

ASTNode *Parser::parse_expression(int min_prec) {
  ASTNode *lhs = parse_primary();

  // 🔁 Handle function application (left-associative, tighter than any
  // operator)
  while (is_primary(current_token.type)) {
    ASTNode *rhs = parse_primary();
    ASTNode *app = new ASTNode{NodeType::FunctionApplication, ""};
    app->children.push_back(lhs);
    app->children.push_back(rhs);
    lhs = app;
  }

  // 🧠 Then handle binary operators
  while (true) {
    int prec = get_precedence(current_token.type);
    if (prec < min_prec)
      break;

    TokenType op = current_token.type;
    consume(op);

    ASTNode *rhs = parse_expression(prec + 1);

    ASTNode *bin = new ASTNode{NodeType::BinOp, token_to_string(op)};
    bin->children.push_back(lhs);
    bin->children.push_back(rhs);

    lhs = bin;
  }

  return lhs;
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
  node->type = NodeType::TypeDef;
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

ASTNode *Parser::parse_type_signature(const Token &identifier) {
  consume(TokenType::DoubleColon);
  auto root = new ASTNode{};
  root->value = identifier.data;
  root->type = NodeType::TypeSignature;

  // consume params and return type
  std::vector<Token> params;
  auto return_type = new ASTNode{};
  return_type->type = NodeType::ReturnType;
  while (current_token.type != TokenType::NewLine) {
    if (current_token.type == TokenType::Type) {
      Token type = consume(TokenType::Type);
      params.emplace_back(type);
    } else if (current_token.type == TokenType::Identifier) {
      Token type = consume(TokenType::Identifier);
      params.emplace_back(type);
    }

    if (current_token.type == TokenType::RightArrow) {
      consume(TokenType::RightArrow);
    }
  }

  if (params.size() == 1) {
    return_type->value = params[0].data;
    root->children.emplace_back(return_type);
  } else {
    for (int i = 0; i < params.size() - 1; ++i) {
      Token current = params[i];
      auto sig_param = new ASTNode{};
      sig_param->type = NodeType::ParamType;
      sig_param->value = current.data;
      root->children.emplace_back(sig_param);
    }
    return_type->value = params[params.size() - 1].data;
    root->children.emplace_back(return_type);
  }

  pending_signatures[identifier.data].push_back(root);

  return root;
}

ASTNode *Parser::parse_function_def(const Token &identifier) {
  auto root = new ASTNode{};
  root->value = identifier.data;
  root->type = NodeType::FunctionDef;

  std::vector<Token> params;
  while (current_token.type != TokenType::Equals) {
    params.emplace_back(consume(TokenType::Identifier));
  }
  consume(TokenType::Equals);

  auto expr = parse_expression(1);
  root->children.emplace_back(expr);

  // Attach all pending signatures (support overloads)
  auto it = pending_signatures.find(identifier.data);
  if (it != pending_signatures.end()) {
    for (ASTNode *sig : it->second) {
      root->children.insert(root->children.begin(), sig);
    }
    pending_signatures.erase(it);
  }
  return root;
}

ASTNode *Parser::parse_assignment(const Token &identifier) {
  fprintf(stdout, "Parsing Assignment.\n");
  fprintf(stdout, "Parsing Equals.\n");
  consume(TokenType::Equals);

  auto value = parse_expression(1);
  auto assignment = new ASTNode{NodeType::Assignment};
  assignment->children.push_back(
      new ASTNode{NodeType::Identifier, identifier.data});
  assignment->children.push_back(value);

  return assignment;
}

ASTNode *Parser::get_function_node(ASTNode *program,
                                   std::string function_name) {
  if (!program || program->type != NodeType::Program) {
    return nullptr;
  }
}

Token Parser::peek_next() {
  size_t old = lexer.cursor;
  Token token = lexer.next();
  lexer.cursor = old;
  return token;
}

ASTNode *Parser::parse_operator_overload(const Token &token) {
  auto root = new ASTNode{};
  root->type = NodeType::OperatorOverload;
  root->value = token.data;
  auto params = new ASTNode{};
  params->type = NodeType::FunctionParams;
  while (current_token.type != TokenType::DoubleColon) {
    params->children.emplace_back(
        new ASTNode{NodeType::ParamType, consume(TokenType::Identifier).data});
  }
  root->children.emplace_back(params);
  consume(TokenType::DoubleColon);
  auto expr = parse_expression(0);
  root->children.emplace_back(expr);

  return root;
}

ASTNode *Parser::parse() {
  advance();
  ASTNode *program = new ASTNode();
  program->type = NodeType::Program;
  while (current_token.type != TokenType::End) {
    bool internal = false;
    if (current_token.type == TokenType::Internal) {
      consume(TokenType::Internal);
      consume(TokenType::NewLine);
      internal = true;
    }
    switch (current_token.type) {
    // possible operator overloading
    case TokenType::LeftParen: {
    } break;
    case TokenType::Input: {
      auto seq = lexer.get_sequence();
      fprintf(stdout, "%s\n", seq);
      program->children.emplace_back(parse_input());
    } break;
    case TokenType::Include: {
      auto seq = lexer.get_sequence();
      program->children.emplace_back(parse_includes());
      fprintf(stderr, "%s\n", seq);
    } break;
    case TokenType::Main: {
      program->children.emplace_back(parse_main_function());
      auto seq = lexer.get_sequence();
      fprintf(stderr, "%s\n", seq);
    } break;
    case TokenType::Identifier: {
      Token identifier = consume(TokenType::Identifier);
      // function signature
      if (current_token.type == TokenType::DoubleColon) {
        auto sig = parse_type_signature(identifier);
        sig->internal = internal;
      } else {
        auto function_def_node = parse_function_def(identifier);
        function_def_node->internal = internal;
        program->children.emplace_back(function_def_node);
      }
    } break;
    case TokenType::Data: {
      auto data = parse_data_definition();
      program->children.emplace_back();
    } break;
    }
    internal = false;
    advance();
  }
  return program;
}
