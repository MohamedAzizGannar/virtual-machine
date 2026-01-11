#ifndef LEXER_H
#define LEXER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef enum {
  TOKEN_OPCODE,
  TOKEN_REGISTER,
  TOKEN_NUMBER,
  TOKEN_LABEL,
  TOKEN_IDENTIFIER,
  TOKEN_COMMA,
  TOKEN_NEWLINE,
  TOKEN_EOF,
  TOKEN_LPAREN,
  TOKEN_RPAREN,
  TOKEN_UNKNOWN
} TokenType;

typedef struct Token {
  TokenType token_type;
  char data[256];
} Token;

int tokenize_line(char *line, Token *tokens);
int get_line(char *data, FILE *fptr);
#endif
