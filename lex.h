#ifndef LEX_H
#define LEX_H

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#undef EOF

typedef enum TokenKind {
  OBRACE, // 0
  CBRACE, // 1
  OPARAN, // 2
  CPARAN, // 3
  SEMICOL, // 4
  KEYWORD, // 5
  ID, // 6
  INTL, // 7
  EOF, // 8
} TokenKind;

typedef struct Token {
  TokenKind kind;
  char *lexeme;
  int value;
} Token;

void initLexer(char *content);
Token *lex();

#endif
