#include "parse.h"
#include "lex.h"

ASTNode *additiveExp();
ASTNode *expr();

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

ASTNode *factor() {
  Token *next = lex();
  if (next->kind == OPARAN) {
    ASTNode *exprN = expr();
    if (lex()->kind != CPARAN) {
      return makeNode(ERR);
    }
    return exprN;
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

ASTNode *relexp() {
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

ASTNode *expr() {
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
  stmt->s1 = expr();
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


