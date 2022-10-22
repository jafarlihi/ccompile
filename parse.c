#include "parse.h"
#include "lex.h"

ASTNode *expression();

ASTNode *makeNode(ASTType type) {
  ASTNode *node = malloc(sizeof(ASTNode));
  node->s1 = NULL;
  node->s2 = NULL;
  node->s3 = NULL;
  node->type = type;
  return node;
}

ASTNode *unaryOp(Token *token) {
  if (token->kind == NEG || token->kind == COMPL || token->kind == LNEG) {
    ASTNode *unaryOp = makeNode(UNARY);
    if (token->kind == NEG)
      unaryOp->fields.charval = '-';
    if (token->kind == COMPL)
      unaryOp->fields.charval = '~';
    if (token->kind == LNEG)
      unaryOp->fields.charval = '!';
    return unaryOp;
  }
  return makeNode(ERR);
}

ASTNode *binaryOp(Token *token) {
  if (token->kind == ADD || token->kind == NEG || token->kind == MUL || token->kind == DIV) {
    ASTNode *binaryOp = makeNode(BINARY);
    if (token->kind == ADD)
      binaryOp->fields.charval = '+';
    if (token->kind == NEG)
      binaryOp->fields.charval = '-';
    if (token->kind == MUL)
      binaryOp->fields.charval = '*';
    if (token->kind == DIV)
      binaryOp->fields.charval = '/';
    return binaryOp;
  }
  return makeNode(ERR);
}

ASTNode *factor() {
  Token *next = lex();
  if (next->kind == OPARAN) {
    ASTNode *expr = expression();
    if (lex()->kind != CPARAN) {
      return makeNode(ERR);
    }
    return expr;
  } else if (next->kind == INTL) {
    ASTNode *expr = makeNode(EXPR);
    expr->fields.intval = next->value;
    expr->exprType = CONSTANT;
    return expr;
  } else {
    ASTNode *unary = unaryOp(next);
    if (unary->type == ERR) return unary;
    ASTNode *factorN = factor();
    ASTNode *expr = makeNode(EXPR);
    expr->exprType = UNARY_OP;
    expr->s1 = unary;
    expr->s2 = factorN;
    return expr;
  }
}

ASTNode *term() {
  ASTNode *factorN = factor();
  Token *next = peek();
  while (next->kind == MUL || next->kind == DIV) {
    ASTNode *binary = binaryOp(lex());
    ASTNode *nextFactor = factor();
    ASTNode *expr = makeNode(EXPR);
    expr->exprType = BINARY_OP;
    expr->s1 = binary;
    expr->s2 = factorN;
    expr->s3 = nextFactor;
    factorN = expr;
    next = peek();
  }
  return factorN;
}

ASTNode *expression() {
  ASTNode *termN = term();
  Token *next = peek();
  while (next->kind == ADD || next->kind == NEG) {
    ASTNode *binary = binaryOp(lex());
    ASTNode *nextTerm = term();
    ASTNode *expr = makeNode(EXPR);
    expr->exprType = BINARY_OP;
    expr->s1 = binary;
    expr->s2 = termN;
    expr->s3 = nextTerm;
    termN = expr;
    next = peek();
  }
  return termN;
}

ASTNode *statement() {
  ASTNode *stmt = makeNode(STMT);
  Token *token = lex();
  if (token->kind != KEYWORD || strcmp(token->lexeme, "return") != 0)
    return makeNode(ERR);
  stmt->stmtType = RETURN;
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


