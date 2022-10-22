#include "codegen.h"

void generate(ASTNode *ast) {
  if (ast->type == FUNC) {
    printf(".globl _%s\n", ast->fields.strval);
    printf("_%s:\n", ast->fields.strval);
  }
  if (ast->type == STMT) {
    if (ast->s1->type == EXPR) {
      printf("movl %d, %%eax\n", ast->s1->fields.intval);
      printf("ret\n");
    }
  }
  if (ast->s1)
    generate(ast->s1);
  if (ast->s2)
    generate(ast->s2);
  if (ast->s3)
    generate(ast->s3);
}

