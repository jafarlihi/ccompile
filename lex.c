#include "lex.h"

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#undef EOF

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

Token *makeStrToken(TokenKind kind, char *lexeme) {
  Token *result = malloc(sizeof(Token));
  result->kind = kind;
  result->lexeme = lexeme;
  return result;
}

Token *lex() {
  while (lexer_content[lexer_position] == ' ' || lexer_content[lexer_position] == '\n')
    lexer_position++;

  if (lexer_content[lexer_position] == '\0')
    return makeToken(EOF);

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
    while (isdigit(lexer_content[lexer_position]))
      lexer_position++;
    return makeToken(INTL);
  }

  if (isalpha(lexer_content[lexer_position])) {
    if (strncmp(lexer_content + lexer_position, "int", 3) == 0) {
      lexer_position += 3;
      if (isspace(lexer_content[lexer_position + 1]) || lexer_content[lexer_position + 1] == '\0')
        return makeStrToken(KEYWORD, "int");
    } else if (strncmp(lexer_content + lexer_position, "return", 6) == 0) {
      lexer_position += 6;
      if (isspace(lexer_content[lexer_position + 1])  || lexer_content[lexer_position + 1] == '\0')
        return makeStrToken(KEYWORD, "return");
    }
  }

  char *lexeme = malloc(64);
  int starting_lexer_position = lexer_position;
  do {
    lexeme[lexer_position - starting_lexer_position] = lexer_content[lexer_position];
    lexer_position++;
  } while(lexer_content[lexer_position] != ' ' && isalpha(lexer_content[lexer_position]));
  lexeme[lexer_position - starting_lexer_position] = '\0';
  return makeStrToken(ID, lexeme);
}

