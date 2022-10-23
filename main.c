#include "lex.h"
#include "parse.h"
#include "codegen.h"
#include <stdbool.h>

char *astTypeToString(ASTType type) {
  switch (type) {
    case EXPR:
      return "EXPR";
    case STMT:
      return "STMT";
    case FUNC:
      return "FUNC";
    case PROG:
      return "PROG";
    case UNARY:
      return "UNARY";
    case BINARY:
      return "BINARY";
    case ERR:
      return "ERR";
  }
}

void printASTNode(ASTNode *node) {
  printf("%s\n", astTypeToString(node->type));
  if (node->s1)
    printASTNode(node->s1);
  if (node->s2)
    printASTNode(node->s2);
  if (node->s3)
    printASTNode(node->s3);
  if (node->ss)
    for (int i = 0; i < node->ss->used; i++) {
      printASTNode(node->ss->array[i]);
    }
}

bool hasError(ASTNode *node) {
  if (node->type == ERR)
    return true;
  if (node->s1 && hasError(node->s1))
    return true;
  if (node->s2 && hasError(node->s2))
    return true;
  if (node->s3 && hasError(node->s3))
    return true;
  if (node->ss)
    for (int i = 0; i < node->ss->used; i++) {
      if (hasError(node->ss->array[i]))
        return true;
    }
  return false;
}

void storeData(const char *filepath, const char *data) {
  FILE *fp = fopen(filepath, "wb");
  if (fp != NULL) {
    fputs(data, fp);
    fclose(fp);
  }
}

int main(int argc, char *argv[]) {
  FILE *f = fopen(argv[1], "r");
  fseek(f, 0L, SEEK_END);
  size_t fsize = ftell(f);
  rewind(f);

  char *fcontent = malloc(fsize + 1);
  fread(fcontent, fsize, 1, f);
  fclose(f);

  /*
     initLexer(fcontent);
     Token *token;
     do {
     token = lex();
     printf("%d", token->kind);
     if (token->kind == KEYWORD || token->kind == ID)
     printf(" - %s", token->lexeme);
     if (token->kind == INTL)
     printf(" - %d", token->value);
     printf("\n");
     } while (token->kind != EOF);
     */

  ASTNode *ast = parse(fcontent);

  //printASTNode(ast);

  char *output = calloc(99999, sizeof(char));
  if (!hasError(ast)) {
    char *path = strdup(argv[1]);
    for (int i = 1; i < strlen(path); i++) {
      if (path[i] == '.') {
        path[i] = '\0';
        break;
      }
    }
    generate(ast, output);
    //printf("%s\n", output);
    storeData("./assembly.s", output);
    char cmd[100] = "gcc -g assembly.s -o ";
    strcat(cmd, path);
    system(cmd);
  }

  return 0;
}
