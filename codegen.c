#include "codegen.h"

void generate(ASTNode *ast, char *output) {
  if (ast->type == FUNC) {
    sprintf(output + strlen(output), ".globl %s\n", ast->fields.strval);
    sprintf(output + strlen(output), "%s:\n", ast->fields.strval);
  }
  if (ast->type == STMT) {
    if (ast->s1->type == EXPR) {
      sprintf(output + strlen(output), "movl $%d, %%eax\n", ast->s1->fields.intval);
      sprintf(output + strlen(output), "ret\n");
    }
  }
  if (ast->s1)
    generate(ast->s1, output);
  if (ast->s2)
    generate(ast->s2, output);
  if (ast->s3)
    generate(ast->s3, output);
}

