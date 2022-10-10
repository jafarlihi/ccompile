#ifndef PARSE_H
#define PARSE_H

#include "lex.h"

typedef enum ASTType {
  EXP,
  STMT,
  FUNC,
  PROG,
  ERR,
} ASTType;

typedef struct ASTNode {
  ASTType type;
  union {
    int intval;
    const char *strval;
  } fields;
  struct ASTNode *s1;
  struct ASTNode *s2;
  struct ASTNode *s3;
} ASTNode;

ASTNode *parse(char *fcontent);

#endif
