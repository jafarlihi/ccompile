#include "lex.h"
#include "parse.h"

int main(int argc, char *argv[]) {
  FILE *f = fopen(argv[1], "r");
  fseek(f, 0L, SEEK_END);
  size_t fsize = ftell(f);
  rewind(f);

  char *fcontent = malloc(fsize + 1);
  fread(fcontent, fsize, 1, f);
  fclose(f);

  initLexer(fcontent);
  Token *token;
  do {
    token = lex();
    printf("%d\n", token->kind);
    if (token->kind == KEYWORD || token->kind == ID)
      printf("%s\n", token->lexeme);
  } while (token->kind != EOF);

  ASTNode *ast = parse(fcontent);

  printf("%d\n", ast->type);

  return 0;
}
