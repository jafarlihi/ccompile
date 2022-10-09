#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#undef EOF

typedef enum TokenKind {
  OBRACE,
  CBRACE,
  OPARAN,
  CPARAN,
  SEMICOL,
  KEYWORD,
  ID,
  INTL,
  EOF,
} TokenKind;

typedef struct Token {
  TokenKind kind;
  char *str_value;
  int value;
} Token;

char *lexer_content;
size_t lexer_position;

void initLexer(char *content) {
  lexer_content = content;
  lexer_position = 0;
}

Token *makeToken(TokenKind kind) {
  Token *result = malloc(sizeof(Token));
  result->kind = kind;
  return result;
}

Token *makeStrToken(TokenKind kind, char *str_value) {
  Token *result = malloc(sizeof(Token));
  result->kind = kind;
  result->str_value = str_value;
  return result;
}

Token *lex() {
  switch (lexer_content[lexer_position]) {
    case '{':
      lexer_position++;
      return makeToken(OBRACE);
    case '}':
      lexer_position++;
      return makeToken(CBRACE);
    case '(':
      lexer_position++;
      return makeToken(OPARAN);
    case ')':
      lexer_position++;
      return makeToken(CPARAN);
    case ';':
      lexer_position++;
      return makeToken(SEMICOL);
  }

  if (isdigit(lexer_content[lexer_position])) {
    // TODO: Read value
    while (isdigit(lexer_content[lexer_position + 1]))
      lexer_position++;
    return makeToken(INTL);
  }

  if (isalpha(lexer_content[lexer_position])) {
    if (strncmp(lexer_content + lexer_position, "int", 3) == 0) {
      lexer_position += 3;
      return makeStrToken(KEYWORD, "int");
    } else if (strncmp(lexer_content + lexer_position, "return", 6) == 0) {
      lexer_position += 6;
      return makeStrToken(KEYWORD, "return");
    }
  }

  char *str_value = malloc(64);
  int starting_lexer_position = lexer_position;
  do {
    str_value[lexer_position - starting_lexer_position] = lexer_content[lexer_position];
    lexer_position++;
  } while(lexer_content[lexer_position] != ' ' && isalpha(lexer_content[lexer_position]));
  str_value[lexer_position - starting_lexer_position] = '\0';
  return makeStrToken(ID, str_value);
}

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
  } while (token->kind != EOF);

  return 0;
}
