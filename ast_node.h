#pragma once
#include <string>
#include <vector>

enum class NodeType {
  Program = 0,
  Identifier = 1,
  Assignment = 2,
  NumberLiteral = 3,
  Field = 4,
  FieldType = 5,
  FieldName = 6,
  AliasList = 7,
  Alias = 8,
  TypeDef = 9,
  FunctionDef = 10,
  FunctionParams = 10,
  Version = 11,
  Tuple = 12,
  List = 13,
};

struct ASTNode {
  NodeType type;
  std::string value;
  std::vector<ASTNode *> children;
};
