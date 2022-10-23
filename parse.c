#include "array.h"
#include "parse.h"
#include "lex.h"

ASTNode *additiveExp();
ASTNode *lorexp();

ASTNode *makeNode(ASTType type) {
  ASTNode *node = malloc(sizeof(ASTNode));
  node->s1 = NULL;
  node->s2 = NULL;
  node->s3 = NULL;
  node->type = type;
  node->fields.intval = 0;
  node->fields.charval = '\0';
  node->fields.strval = "\0";
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
  ASTNode *binaryOp = makeNode(BINARY);
  switch (token->kind) {
    case ADD:
      binaryOp->fields.charval = '+';
      break;
    case NEG:
      binaryOp->fields.charval = '-';
      break;
    case MUL:
      binaryOp->fields.charval = '*';
      break;
    case DIV:
      binaryOp->fields.charval = '/';
      break;
    case AND:
      binaryOp->fields.strval = "&&";
      break;
    case OR:
      binaryOp->fields.strval = "||";
      break;
    case EQ:
      binaryOp->fields.strval = "==";
      break;
    case NEQ:
      binaryOp->fields.strval = "!=";
      break;
    case LE:
      binaryOp->fields.charval = '<';
      break;
    case LEE:
      binaryOp->fields.strval = "<=";
      break;
    case GE:
      binaryOp->fields.charval = '>';
      break;
    case GEE:
      binaryOp->fields.strval = ">=";
      break;
  }
  return binaryOp->fields.charval || binaryOp->fields.strval ? binaryOp : makeNode(ERR);
}

ASTNode *ident() {

  #ifdef DEBUG
  printf("%s\n", __func__);
  #endif

  Token *next = lex();
  if (next->kind != ID)
    return makeNode(ERR);
  ASTNode *identN = makeNode(IDENT);
  identN->fields.strval = next->lexeme;
  return identN;
}

ASTNode *pexpression() {

  #ifdef DEBUG
  printf("%s\n", __func__);
  #endif

  Token *next = peek();
  Token *nextNext = peekSecond();
  if (next->kind == ID && nextNext->kind == ASNG) {
    ASTNode *identN = ident();
    next = lex();
    if (next->kind != ASNG)
      return makeNode(ERR);
    ASTNode *expr = pexpression();
    ASTNode *res = makeNode(EXPR);
    res->exprType = VAR_ASSNG;
    res->s1 = identN;
    res->s2 = expr;
    return expr;
  }
  return lorexp();
}

ASTNode *factor() {

  #ifdef DEBUG
  printf("%s\n", __func__);
  #endif

  Token *next = peek();
  if (next->kind == OPARAN) {
    lex();
    ASTNode *exprN = pexpression();
    if (lex()->kind != CPARAN) {
      return makeNode(ERR);
    }
    return exprN;
  } else if (next->kind == INTL) {
    lex();
    ASTNode *expr = makeNode(EXPR);
    expr->fields.intval = next->value;
    expr->exprType = CONSTANT;
    return expr;
  } else if (next->kind == ID) {
    return ident();
  } else {
    lex();
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

ASTNode *relexp() {

  #ifdef DEBUG
  printf("%s\n", __func__);
  #endif

  ASTNode *addexpN = additiveExp();
  Token *next = peek();
  while (next->kind == LE || next->kind == LEE || next->kind == GE || next->kind == GEE) {
    ASTNode *binary = binaryOp(lex());
    ASTNode *nextAddexp = additiveExp();
    ASTNode *expr = makeNode(EXPR);
    expr->exprType = BINARY_OP;
    expr->s1 = binary;
    expr->s2 = addexpN;
    expr->s3 = nextAddexp;
    addexpN = expr;
    next = peek();
  }
  return addexpN;
}

ASTNode *eqexp() {

  #ifdef DEBUG
  printf("%s\n", __func__);
  #endif

  ASTNode *relexpN = relexp();
  Token *next = peek();
  while (next->kind == NEQ || next->kind == EQ) {
    ASTNode *binary = binaryOp(lex());
    ASTNode *nextRelexp = relexp();
    ASTNode *expr = makeNode(EXPR);
    expr->exprType = BINARY_OP;
    expr->s1 = binary;
    expr->s2 = relexpN;
    expr->s3 = nextRelexp;
    relexpN = expr;
    next = peek();
  }
  return relexpN;
}

ASTNode *landexp() {

  #ifdef DEBUG
  printf("%s\n", __func__);
  #endif

  ASTNode *eqexpN = eqexp();
  Token *next = peek();
  while (next->kind == AND) {
    ASTNode *binary = binaryOp(lex());
    ASTNode *nextEqexp = eqexp();
    ASTNode *expr = makeNode(EXPR);
    expr->exprType = BINARY_OP;
    expr->s1 = binary;
    expr->s2 = eqexpN;
    expr->s3 = nextEqexp;
    eqexpN = expr;
    next = peek();
  }
  return eqexpN;
}

ASTNode *lorexp() {

  #ifdef DEBUG
  printf("%s\n", __func__);
  #endif

  ASTNode *landexpN = landexp();
  Token *next = peek();
  while (next->kind == OR) {
    ASTNode *binary = binaryOp(lex());
    ASTNode *nextLandexp = landexp();
    ASTNode *expr = makeNode(EXPR);
    expr->exprType = BINARY_OP;
    expr->s1 = binary;
    expr->s2 = landexpN;
    expr->s3 = nextLandexp;
    landexpN = expr;
    next = peek();
  }
  return landexpN;
}

ASTNode *term() {

  #ifdef DEBUG
  printf("%s\n", __func__);
  #endif

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

ASTNode *additiveExp() {

  #ifdef DEBUG
  printf("%s\n", __func__);
  #endif

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

  #ifdef DEBUG
  printf("%s\n", __func__);
  #endif

  ASTNode *stmt = makeNode(STMT);
  Token *token = peek();
  if (token->kind == KEYWORD && strcmp(token->lexeme, "return") == 0) {
    lex();
    stmt->stmtType = RETURN;
    stmt->s1 = pexpression();
  } else if (token->kind == ID) {
    stmt->stmtType = EXPR_S;
    stmt->s1 = pexpression();
  } else if (token->kind == KEYWORD && strcmp(token->lexeme, "int") == 0) {
    lex();
    stmt->stmtType = VARASSNG;
    stmt->s1 = ident();
    if (peek()->kind == ASNG) {
      lex();
      stmt->s2 = pexpression();
    }
  }
  if (lex()->kind != SEMICOL)
    return makeNode(ERR);
  return stmt;
}

ASTNode *function() {

  #ifdef DEBUG
  printf("%s\n", __func__);
  #endif

  ASTNode *func = makeNode(FUNC);
  Token *token = lex();
  if (token->kind == KEYWORD && strcmp(token->lexeme, "int") == 0) {
    token = lex();
    func->fields.strval = token->lexeme;
    if (token->kind == ID && lex()->kind == OPARAN && lex()->kind == CPARAN && lex()->kind == OBRACE) {
      Array *ss = malloc(sizeof(Array));
      initArray(ss, 10);
      func->ss = ss;
      while (peek()->kind == KEYWORD || peek()->kind == ID) {
        ASTNode *stmt = statement();
        insertArray(func->ss, stmt);
      }
      if (lex()->kind != CBRACE) {
        return makeNode(ERR);
      }
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


