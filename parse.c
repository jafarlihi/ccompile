#include "parse.h"
#include "lex.h"

ASTNode *makeNode(ASTType type) {
  ASTNode *node = malloc(sizeof(ASTNode));
  node->s1 = NULL;
  node->s2 = NULL;
  node->s3 = NULL;
  node->type = type;
  return node;
}

ASTNode *expression() {
  ASTNode *expr = makeNode(EXPR);
  Token *token = lex();
  if (token->kind != INTL)
    return makeNode(ERR);
  expr->fields.intval = token->value;
  return expr;
}

ASTNode *statement() {
  ASTNode *stmt = makeNode(STMT);
  Token *token = lex();
  if (token->kind != KEYWORD || strcmp(token->lexeme, "return") != 0)
    return makeNode(ERR);
  stmt->s1 = expression();
  if (lex()->kind != SEMICOL)
    return makeNode(ERR);
  return stmt;
}

ASTNode *function() {
  ASTNode *func = makeNode(FUNC);
  Token *token = lex();
  if (token->kind == KEYWORD && strcmp(token->lexeme, "int") == 0) {
    token = lex();
    func->fields.strval = token->lexeme;
    if (token->kind == ID && lex()->kind == OPARAN && lex()->kind == CPARAN && lex()->kind == OBRACE) {
      ASTNode *stmt = statement();
      func->s1 = stmt;
      if (lex()->kind != CBRACE)
        return makeNode(ERR);
    } else {
      return makeNode(ERR);
    }
  } else {
    return makeNode(ERR);
  }
  return func;
}

ASTNode *program() {
  ASTNode *prog = makeNode(PROG);
  prog->s1 = function();
  return prog;
}

ASTNode *parse(char *fcontent) {
  initLexer(fcontent);
  return program();
}


