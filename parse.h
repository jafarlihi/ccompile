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
  IDENT,
  ERR,
} ASTType;

typedef enum ExprType {
  CONSTANT,
  UNARY_OP,
  BINARY_OP,
  VAR_ASSNG,
} ExprType;

typedef enum StmtType {
  RETURN,
  EXPR_S,
  VARASSNG,
} StmtType;

typedef struct Array Array;
typedef struct ASTNode ASTNode;

struct ASTNode {
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
  int stackIndex;
  Array *ss;
};

ASTNode *parse(char *fcontent);

#endif
