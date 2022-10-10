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

ASTNode *makeErr() {
  ASTNode *err = malloc(sizeof(ASTNode));
  err->type = ERR;
  return err;
}

ASTNode *expression() {
  ASTNode *expr = malloc(sizeof(ASTNode));
  Token *token = lex();
  if (token->kind != INTL)
    return makeErr();
  // expr->fields->intval = token->value;
  return expr;
}

ASTNode *statement() {
  ASTNode *stmt = malloc(sizeof(ASTNode));
  Token *token = lex();
  if (token->kind != KEYWORD || strcmp(token->lexeme, "return") != 0)
    return makeErr();
  stmt->s1 = expression();
  if (lex()->kind != SEMICOL)
    return makeErr();
  return stmt;
}

ASTNode *function() {
  ASTNode *func = malloc(sizeof(ASTNode));
  Token *token = lex();
  if (token->kind == KEYWORD && strcmp(token->lexeme, "int") == 0) {
    token = lex();
    func->fields.strval = token->lexeme;
    if (token->kind == ID && lex()->kind == OPARAN && lex()->kind == CPARAN && lex()->kind == OBRACE) {
      ASTNode *stmt = statement();
      func->s1 = stmt;
      if (lex()->kind != CBRACE)
        return makeErr();
    } else {
      return makeErr();
    }
  } else {
    return makeErr();
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


