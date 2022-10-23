#include "codegen.h"
#include "array.h"
#include <stdbool.h>

int labelCounter = 0;

void expression(ASTNode *ast, char *output) {
  if (ast->exprType == UNARY_OP) {
    if (ast->s2)
      expression(ast->s2, output);
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
  } else if (ast->exprType == BINARY_OP && strcmp(ast->s1->fields.strval, "||") != 0 && strcmp(ast->s1->fields.strval, "&&") != 0) {
    expression(ast->s2, output);
    sprintf(output + strlen(output), "push %%rax\n");
    expression(ast->s3, output);
    if (ast->s1->fields.charval == '-') {
      sprintf(output + strlen(output), "movl %%eax, %%ecx\n");
      sprintf(output + strlen(output), "pop %%rax\n");
      sprintf(output + strlen(output), "subl %%ecx, %%eax\n");
    }
    if (ast->s1->fields.charval == '+') {
      sprintf(output + strlen(output), "pop %%rcx\n");
      sprintf(output + strlen(output), "addl %%ecx, %%eax\n");
    }
    if (ast->s1->fields.charval == '*') {
      sprintf(output + strlen(output), "pop %%rcx\n");
      sprintf(output + strlen(output), "imul %%ecx, %%eax\n");
    }
    if (ast->s1->fields.charval == '/') {
      sprintf(output + strlen(output), "movl %%eax, %%ecx\n");
      sprintf(output + strlen(output), "pop %%rax\n");
      sprintf(output + strlen(output), "cdq\n");
      sprintf(output + strlen(output), "idivl %%ecx\n");
    }
    if (strcmp(ast->s1->fields.strval, "==") == 0) {
      sprintf(output + strlen(output), "pop %%rcx\n");
      sprintf(output + strlen(output), "cmpl %%eax, %%ecx\n");
      sprintf(output + strlen(output), "movl $0, %%eax\n");
      sprintf(output + strlen(output), "sete %%al\n");
    }
    if (strcmp(ast->s1->fields.strval, "!=") == 0) {
      sprintf(output + strlen(output), "pop %%rcx\n");
      sprintf(output + strlen(output), "cmpl %%eax, %%ecx\n");
      sprintf(output + strlen(output), "movl $0, %%eax\n");
      sprintf(output + strlen(output), "setne %%al\n");
    }
    if (ast->s1->fields.charval == '<') {
      sprintf(output + strlen(output), "pop %%rcx\n");
      sprintf(output + strlen(output), "cmpl %%eax, %%ecx\n");
      sprintf(output + strlen(output), "movl $0, %%eax\n");
      sprintf(output + strlen(output), "setl %%al\n");
    }
    if (strcmp(ast->s1->fields.strval, "<=") == 0) {
      sprintf(output + strlen(output), "pop %%rcx\n");
      sprintf(output + strlen(output), "cmpl %%eax, %%ecx\n");
      sprintf(output + strlen(output), "movl $0, %%eax\n");
      sprintf(output + strlen(output), "setle %%al\n");
    }
    if (ast->s1->fields.charval == '>') {
      sprintf(output + strlen(output), "pop %%rcx\n");
      sprintf(output + strlen(output), "cmpl %%eax, %%ecx\n");
      sprintf(output + strlen(output), "movl $0, %%eax\n");
      sprintf(output + strlen(output), "setg %%al\n");
    }
    if (strcmp(ast->s1->fields.strval, ">=") == 0) {
      sprintf(output + strlen(output), "pop %%rcx\n");
      sprintf(output + strlen(output), "cmpl %%eax, %%ecx\n");
      sprintf(output + strlen(output), "movl $0, %%eax\n");
      sprintf(output + strlen(output), "setge %%al\n");
    }
  } else if (ast->exprType == BINARY_OP && strcmp(ast->s1->fields.strval, "||") == 0) {
    expression(ast->s2, output);
    int clauseLabel = labelCounter++;
    int endLabel = labelCounter++;
    sprintf(output + strlen(output), "cmpl $0, %%eax\n");
    sprintf(output + strlen(output), "je _l%d\n", clauseLabel);
    sprintf(output + strlen(output), "movl $1, %%eax\n");
    sprintf(output + strlen(output), "jmp _l%d\n", endLabel);
    sprintf(output + strlen(output), "_l%d:\n", clauseLabel);
    expression(ast->s3, output);
    sprintf(output + strlen(output), "cmpl $0, %%eax\n");
    sprintf(output + strlen(output), "movl $0, %%eax\n");
    sprintf(output + strlen(output), "setne %%al\n");
    sprintf(output + strlen(output), "_l%d:\n", endLabel);
  } else if (ast->exprType == BINARY_OP && strcmp(ast->s1->fields.strval, "&&") == 0) {
    int clauseLabel = labelCounter++;
    int endLabel = labelCounter++;
    expression(ast->s2, output);
    sprintf(output + strlen(output), "cmpl $0, %%eax\n");
    sprintf(output + strlen(output), "jne _l%d\n", clauseLabel);
    sprintf(output + strlen(output), "jmp _l%d\n", endLabel);
    sprintf(output + strlen(output), "_l%d:\n", clauseLabel);
    expression(ast->s3, output);
    sprintf(output + strlen(output), "cmpl $0, %%eax\n");
    sprintf(output + strlen(output), "movl $0, %%eax\n");
    sprintf(output + strlen(output), "setne %%al\n");
    sprintf(output + strlen(output), "_l%d:\n", endLabel);
  }
}

Array *vars;

void initCodegen() {
  vars = malloc(sizeof(Array));
  initArray(vars, 10);
}

bool shouldRet = false;
int stackIndex = 0;

void generate(ASTNode *ast, char *output) {
  if (ast->type == FUNC) {
    sprintf(output + strlen(output), ".globl %s\n", ast->fields.strval);
    sprintf(output + strlen(output), "%s:\n", ast->fields.strval);
    sprintf(output + strlen(output), "push %%rbp\n");
    sprintf(output + strlen(output), "movq %%rsp, %%rbp\n");
  }
  if (ast->type == STMT && ast->stmtType == RETURN) {
    expression(ast->s1, output);
  }
  if (ast->type == STMT && ast->stmtType == VARASSNG) {
    for (int i = 0; i < vars->used; i++) {
      if (strcmp(vars->array[i]->fields.strval, ast->s1->fields.strval) == 0) {
        printf("Can't declare variable twice\n");
        exit(1);
      }
    }
    expression(ast->s2, output);
    sprintf(output + strlen(output), "push %%eax\n");
    ast->s1->stackIndex = stackIndex;
    insertArray(vars, ast->s1);
    stackIndex -= 4;
  }
  if (ast->type == EXPR_S && ast->stmtType == VARASSNG) {
    expression(ast->s1, output);
    int offset = -1;
    for (int i = 0; i < vars->used; i++) {
      if (strcmp(vars->array[i]->fields.strval, ast->s1->fields.strval) == 0) {
        offset = vars->array[i]->stackIndex;
      }
    }
    if (offset == -1) {
      printf("Can't resolve variable\n");
      exit(1);
    }
    sprintf(output + strlen(output), "movl %%eax, %d(%%ebp)\n", offset);
  }
  if (ast->s1)
    generate(ast->s1, output);
  if (ast->s2)
    generate(ast->s2, output);
  if (ast->s3)
    generate(ast->s3, output);
  if (ast->ss)
    for (int i = 0; i < ast->ss->used; i++) {
      generate(ast->ss->array[i], output);
    }
  if (ast->type == STMT && ast->stmtType == RETURN)
    shouldRet = true;
  if (ast->type == FUNC) {
    sprintf(output + strlen(output), "movq %%rbp, %%rsp\n");
    sprintf(output + strlen(output), "pop %%rbp\n");
    if (shouldRet)
      sprintf(output + strlen(output), "ret\n");
  }
}

