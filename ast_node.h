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
  FunctionApplication = 11,
  TypeSignature = 12,
  FunctionParams = 13,
  Version = 14,
  Tuple = 15,
  List = 16,
  Input = 17,
  Output = 18,
  Include = 19,
  Uniform = 20,
  ReturnType = 21,
  ParamType = 22,
  MemberAccess = 23,
  BinOp = 24,
  EntryPoint = 25,
  LetInExpr = 26,
  LetBinding = 27,
  FieldAccess = 28,
  OperatorOverload = 29,
  Let = 30
};

struct ASTNode {
  NodeType type;
  std::string value;
  std::vector<ASTNode *> children;
  bool internal = false;
};
