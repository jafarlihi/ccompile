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
  OBRACE,
  CBRACE,
  OPARAN,
  CPARAN,
  SEMICOL,
  KEYWORD,
  ID,
  INTL,
  NEG,
  COMPL,
  LNEG,
  EOF,
} TokenKind;

typedef struct Token {
  TokenKind kind;
  char *lexeme;
  int value;
} Token;

void initLexer(char *content);
Token *lex();

#endif
