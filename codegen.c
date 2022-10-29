#include "codegen.h"
#include "array.h"
#include <stdbool.h>

Array *vars = NULL;

void initCodegen() {
  vars = malloc(sizeof(Array));
  initArray(vars, 10);
}

int labelCounter = 0;
bool shouldRet = false;
int stackIndex = 0;


void expression(ASTNode *ast, char *output) {
  if (ast->exprType == UNARY_OP) {
    if (ast->s2)
      expression(ast->s2, output);
    if (ast->s1->fields.charval == '-')
      sprintf(output + strlen(output), "neg %%rax\n");
    else if (ast->s1->fields.charval == '~')
      sprintf(output + strlen(output), "not %%rax\n");
    else if (ast->s1->fields.charval == '!') {
      sprintf(output + strlen(output), "cmpq $0, %%rax\n");
      sprintf(output + strlen(output), "movq $0, %%rax\n");
      sprintf(output + strlen(output), "sete %%al\n");
    }
  } else if (ast->exprType == CONSTANT) {
    sprintf(output + strlen(output), "movq $%d, %%rax\n", ast->fields.intval);
  } else if (ast->type == IDENT) {
     for (int i = 0; i < vars->used; i++) {
      if (strcmp(vars->array[i]->fields.strval, ast->fields.strval) == 0) {
        sprintf(output + strlen(output), "movq %d(%%rbp), %%rax\n", vars->array[i]->stackIndex);
        return;
      }
    }
    printf("Can't resolve variable\n");
    exit(1);
  } else if (ast->exprType == BINARY_OP && strcmp(ast->s1->fields.strval, "||") != 0 && strcmp(ast->s1->fields.strval, "&&") != 0) {
    expression(ast->s2, output);
    sprintf(output + strlen(output), "push %%rax\n");
    expression(ast->s3, output);
    if (ast->s1->fields.charval == '-') {
      sprintf(output + strlen(output), "movq %%rax, %%rcx\n");
      sprintf(output + strlen(output), "pop %%rax\n");
      sprintf(output + strlen(output), "subq %%rcx, %%rax\n");
    }
    if (ast->s1->fields.charval == '+') {
      sprintf(output + strlen(output), "pop %%rcx\n");
      sprintf(output + strlen(output), "addq %%rcx, %%rax\n");
    }
    if (ast->s1->fields.charval == '*') {
      sprintf(output + strlen(output), "pop %%rcx\n");
      sprintf(output + strlen(output), "imul %%rcx, %%rax\n");
    }
    if (ast->s1->fields.charval == '/') {
      sprintf(output + strlen(output), "movq %%rax, %%rcx\n");
      sprintf(output + strlen(output), "pop %%rax\n");
      sprintf(output + strlen(output), "cqo\n");
      sprintf(output + strlen(output), "idivq %%rcx\n");
    }
    if (strcmp(ast->s1->fields.strval, "==") == 0) {
      sprintf(output + strlen(output), "pop %%rcx\n");
      sprintf(output + strlen(output), "cmpq %%rax, %%rcx\n");
      sprintf(output + strlen(output), "movq $0, %%rax\n");
      sprintf(output + strlen(output), "sete %%al\n");
    }
    if (strcmp(ast->s1->fields.strval, "!=") == 0) {
      sprintf(output + strlen(output), "pop %%rcx\n");
      sprintf(output + strlen(output), "cmpq %%rax, %%rcx\n");
      sprintf(output + strlen(output), "movq $0, %%rax\n");
      sprintf(output + strlen(output), "setne %%al\n");
    }
    if (ast->s1->fields.charval == '<') {
      sprintf(output + strlen(output), "pop %%rcx\n");
      sprintf(output + strlen(output), "cmpq %%rax, %%rcx\n");
      sprintf(output + strlen(output), "movq $0, %%rax\n");
      sprintf(output + strlen(output), "setl %%al\n");
    }
    if (strcmp(ast->s1->fields.strval, "<=") == 0) {
      sprintf(output + strlen(output), "pop %%rcx\n");
      sprintf(output + strlen(output), "cmpq %%rax, %%rcx\n");
      sprintf(output + strlen(output), "movq $0, %%rax\n");
      sprintf(output + strlen(output), "setle %%al\n");
    }
    if (ast->s1->fields.charval == '>') {
      sprintf(output + strlen(output), "pop %%rcx\n");
      sprintf(output + strlen(output), "cmpq %%rax, %%rcx\n");
      sprintf(output + strlen(output), "movq $0, %%rax\n");
      sprintf(output + strlen(output), "setg %%al\n");
    }
    if (strcmp(ast->s1->fields.strval, ">=") == 0) {
      sprintf(output + strlen(output), "pop %%rcx\n");
      sprintf(output + strlen(output), "cmpq %%rax, %%rcx\n");
      sprintf(output + strlen(output), "movq $0, %%rax\n");
      sprintf(output + strlen(output), "setge %%al\n");
    }
  } else if (ast->exprType == BINARY_OP && strcmp(ast->s1->fields.strval, "||") == 0) {
    // TODO: Convert to 64bit
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
    sprintf(output + strlen(output), "push %%rax\n");
    ast->s1->stackIndex = stackIndex;
    insertArray(vars, ast->s1);
    stackIndex -= 4;
  }
  if (ast->type == STMT && ast->stmtType == EXPR_S) {
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
    sprintf(output + strlen(output), "movq %%rax, %d(%%rbp)\n", offset);
  }
  if (ast->type == IDENT) {
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
    sprintf(output + strlen(output), "movq %d(%%rbp), %%rax\n", offset);
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

