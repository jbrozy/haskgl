#include <iostream>

#define FLAG_IMPLEMENTATION
#include "Lexer.h"
#include "flag.h"
#include "parser.h"
#include "ast_node.h"

#include <fstream>
#include <sstream>

void usage(void) {
  fprintf(stderr, "Usage: %s [OPTIONS]\n", flag_program_name());
  flag_print_options(stderr);
}

std::string type_to_string(NodeType type) {
  switch (type) {
  case Version:
    return "Version";
  case Program:
    return "Program";
  case Ident:
    return "Identifier";
  case Assignment:
    return "Assignment";
  case NumberLiteral:
    return "NumberLiteral";
  case TypeDef:
    return "TypeDef";
  case FieldType:
    return "FieldType";
  case FieldName:
    return "FieldName";
  case Field:
    return "Field";
  case AliasList:
    return "AliasList";
  case Alias:
    return "Alias";
  }
}

void printAST(const ASTNode *node, int indent = 0) {
  if (!node)
    return;

  std::string pad(indent * 2, ' ');
  std::cout << pad << "- " << type_to_string(node->type);

  if (!node->value.empty()) {
    std::cout << " (" << node->value << ")";
  }

  std::cout << "\n";

  for (const ASTNode *child : node->children) {
    printAST(child, indent + 1);
  }
}

int main(int argc, char **argv) {
  std::ifstream stream("assets/data.hgl");
  std::stringstream buffer;
  buffer << stream.rdbuf();

  std::string strbf = buffer.str();
  const char *source = strbf.c_str();
  fprintf(stdout, "%s\n", source);
  stream.close();

  Lexer lexer{source};

  Parser parser{lexer};
  ASTNode *root = parser.parse();
  printAST(root);
  return 0;
}
