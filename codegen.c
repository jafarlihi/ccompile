#include "codegen.h"

void expr(ASTNode *ast, char *output) {
  if (ast->exprType == UNARY_OP) {
    if (ast->s2)
      expr(ast->s2, output);
    if (ast->s1->fields.charval == '-')
      sprintf(output + strlen(output), "neg %%eax\n");
    else if (ast->s1->fields.charval == '~')
      sprintf(output + strlen(output), "not %%eax\n");
    else if (ast->s1->fields.charval == '!') {
      sprintf(output + strlen(output), "cmpl $0, %%eax\n");
      sprintf(output + strlen(output), "movl $0, %%eax\n");
      sprintf(output + strlen(output), "sete %%al\n");
    }
  } else if (ast->exprType == CONSTANT) {
    sprintf(output + strlen(output), "movl $%d, %%eax\n", ast->fields.intval);
  }
}

void generate(ASTNode *ast, char *output) {
  if (ast->type == FUNC) {
    sprintf(output + strlen(output), ".globl %s\n", ast->fields.strval);
    sprintf(output + strlen(output), "%s:\n", ast->fields.strval);
  }
  if (ast->type == STMT) {
    expr(ast->s1, output);
  }
  if (ast->s1)
    generate(ast->s1, output);
  if (ast->s2)
    generate(ast->s2, output);
  if (ast->s3)
    generate(ast->s3, output);

  if (ast->type == STMT && ast->stmtType == RETURN)
    sprintf(output + strlen(output), "ret\n");
}

