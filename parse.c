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

ASTNode *function() {
  Token *token = lex();
  if (token->kind == KEYWORD && strcmp(token->lexeme, "int") == 0) {

  }
}

ASTNode *program() {
  ASTNode *prog = malloc(sizeof(ASTNode));
  prog->type = PROG;
  prog->s1 = function();
  return prog;
}

ASTNode *parse(char *fcontent) {
  initLexer(fcontent);
  return program();
}


