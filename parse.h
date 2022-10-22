#ifndef PARSE_H
#define PARSE_H

#include "lex.h"

typedef enum ASTType {
  EXPR,
  STMT,
  FUNC,
  PROG,
  UNARY,
  BINARY,
  ERR,
} ASTType;

typedef enum ExprType {
  CONSTANT,
  UNARY_OP,
  BINARY_OP,
} ExprType;

typedef enum StmtType {
  RETURN,
} StmtType;

typedef struct ASTNode {
  ASTType type;
  union {
    int intval;
    const char *strval;
    char charval;
  } fields;
  ExprType exprType;
  StmtType stmtType;
  struct ASTNode *s1;
  struct ASTNode *s2;
  struct ASTNode *s3;
} ASTNode;

ASTNode *parse(char *fcontent);

#endif
