#pragma once
#include <vector>
#include <string>

enum NodeType {
  Program = 0,
  Ident = 1,
  Assignment = 2,
  NumberLiteral = 3,
  Field = 4,
  FieldType = 5,
  FieldName = 6,
  AliasList = 7,
  Alias = 8,
  TypeDef = 9,
  Version = 10,
};

struct ASTNode {
  NodeType type;
  std::string value;
  std::vector<ASTNode *> children;
};
