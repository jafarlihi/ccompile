#include "lex.h"

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

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

Token *makeIntToken(TokenKind kind, int value) {
  Token *result = malloc(sizeof(Token));
  result->kind = kind;
  result->value = value;
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
    case '-':
      lexer_position++;
      return makeToken(NEG);
    case '~':
      lexer_position++;
      return makeToken(COMPL);
    case '!':
      lexer_position++;
      if (lexer_content[lexer_position] == '=') {
        lexer_position++;
        return makeToken(NEQ);
      }
      return makeToken(LNEG);
    case '+':
      lexer_position++;
      return makeToken(ADD);
    case '*':
      lexer_position++;
      return makeToken(MUL);
    case '/':
      lexer_position++;
      return makeToken(DIV);
    case '&':
      lexer_position++;
      if (lexer_content[lexer_position] == '&') {
        lexer_position++;
        return makeToken(AND);
      }
      return makeToken(AMP);
    case '|':
      lexer_position++;
      if (lexer_content[lexer_position] == '|') {
        lexer_position++;
        return makeToken(OR);
      }
      return makeToken(PIPE);
    case '=':
      lexer_position++;
      if (lexer_content[lexer_position] == '=') {
        lexer_position++;
        return makeToken(EQ);
      }
      return makeToken(ASNG);
    case '<':
      lexer_position++;
      if (lexer_content[lexer_position] == '=') {
        lexer_position++;
        return makeToken(LEE);
      }
      return makeToken(LE);
    case '>':
      lexer_position++;
      if (lexer_content[lexer_position] == '=') {
        lexer_position++;
        return makeToken(GEE);
      }
      return makeToken(GE);
  }

  if (isdigit(lexer_content[lexer_position])) {
    int value = strtol(lexer_content + lexer_position, NULL, 10);
    while (isdigit(lexer_content[lexer_position]))
      lexer_position++;
    return makeIntToken(INTL, value);
  }

  if (isalpha(lexer_content[lexer_position])) {
    if (strncmp(lexer_content + lexer_position, "int", 3) == 0) {
      if (isspace(lexer_content[lexer_position + 3]) || lexer_content[lexer_position + 3] == '\0') {
        lexer_position += 3;
        return makeStrToken(KEYWORD, "int");
      }
    } else if (strncmp(lexer_content + lexer_position, "return", 6) == 0) {
      if (isspace(lexer_content[lexer_position + 6]) || lexer_content[lexer_position + 6] == '\0' || lexer_content[lexer_position + 6] == ';') {
        lexer_position += 6;
        return makeStrToken(KEYWORD, "return");
      }
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

Token *peek() {
  Token *lexed = lex();
  if (lexed->kind == ID || lexed->kind == KEYWORD) {
    lexer_position -= strlen(lexed->lexeme);
  } else if (lexed->kind == INTL) {
    if (lexed->value == 0)
      lexer_position -= 1;
    else
      lexer_position -= floor(log10(abs(lexed->value))) + 1;
  } else if (lexed->kind == AND || lexed->kind == OR || lexed->kind == EQ || lexed->kind == NEQ || lexed->kind == LEE || lexed->kind == GEE) {
    lexer_position -= 2;
  } else {
    lexer_position--;
  }
  return lexed;
}

int countSpaces() {
  int pos = lexer_position + 1;
  while (lexer_content[pos] == ' ' || lexer_content[pos] == '\n') {
    pos++;
  }
  return pos - lexer_position - 1;
}

Token *peekSecond() {
  Token *lexed = lex();
  Token *lexed2 = lex();
  if (lexed->kind == ID || lexed->kind == KEYWORD) {
    lexer_position -= strlen(lexed->lexeme);
  } else if (lexed->kind == INTL) {
    if (lexed->value == 0)
      lexer_position -= 1;
    else
      lexer_position -= floor(log10(abs(lexed->value))) + 1;
  } else if (lexed->kind == AND || lexed->kind == OR || lexed->kind == EQ || lexed->kind == NEQ || lexed->kind == LEE || lexed->kind == GEE) {
    lexer_position -= 2;
  } else {
    lexer_position--;
  }
  int spaces = countSpaces();
  if (lexed2->kind == ID || lexed2->kind == KEYWORD) {
    lexer_position -= strlen(lexed2->lexeme);
  } else if (lexed2->kind == INTL) {
    if (lexed2->value == 0)
      lexer_position -= 1;
    else
      lexer_position -= floor(log10(abs(lexed2->value))) + 1;
  } else if (lexed2->kind == AND || lexed2->kind == OR || lexed2->kind == EQ || lexed2->kind == NEQ || lexed2->kind == LEE || lexed2->kind == GEE) {
    lexer_position -= 2;
  } else {
    lexer_position--;
  }
  lexer_position -= spaces;
  return lexed2;
}

