#include "lex.h"
#include "parse.h"
#include "codegen.h"

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

  generate(ast);

  return 0;
}
