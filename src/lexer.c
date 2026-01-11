#include "../include/lexer.h"
#include "../include/opcodes.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int tokenize_line(char *line, Token *tokens) {
  int pos = 0;
  int len = strlen(line);
  int token_count = 0;
  // Handle Words
  while (pos < len) {
    while (pos < len && isspace(line[pos]))
      pos++;
    if (pos >= len)
      break;
    if (isalnum(line[pos])) {
      int is_register = 0;
      if (line[pos] == 'r')
        is_register = 1;

      int start = pos;
      while (pos < len && (isalnum(line[pos]) || line[pos] == '_')) {
        pos++;
      }
      int word_len = pos - start;

      strncpy(tokens[token_count].data, &line[start], word_len);
      tokens[token_count].data[word_len] = '\0';

      if (pos < len && line[pos] == ':') {

        tokens[token_count].token_type = TOKEN_LABEL;
        pos++;
      } else if (is_register)
        tokens[token_count].token_type = TOKEN_REGISTER;
      else {
        // TODO :Check if OPCODE or IDENTIFIER
        for (int i = 0; i < OP_CODES_COUNT; i++) {
          if (strcmp(tokens[token_count].data, OP_CODES[i]) == 0)
            tokens[token_count].token_type = TOKEN_OPCODE;
        }
        if (!tokens[token_count].token_type)
          tokens[token_count].token_type = TOKEN_IDENTIFIER;
      }
      token_count++;
      continue;
    }
    // Handle Numbers
    if (isdigit(line[pos]) || line[pos] == '#') {
      if (line[pos] == '#')
        pos++;

      int start = pos;
      while (pos < len &&
             (isdigit(line[pos]) || line[pos] == 'x' || line[pos] == 'X'))
        pos++;
      int num_len = pos - start;
      strncpy(tokens[token_count].data, &line[start], num_len);
      tokens[token_count].data[num_len] = '\0';
      tokens[token_count].token_type = TOKEN_NUMBER;
      token_count++;
      continue;
    }
    // Specific Cases
    switch (line[pos]) {
    case ',':
      strcpy(tokens[token_count].data, ",");
      tokens[token_count].token_type = TOKEN_COMMA;
      pos++;
      token_count++;
      break;

    case '\n':
      tokens[token_count].token_type = TOKEN_NEWLINE;
      strcpy(tokens[token_count].data, "\n");
      token_count++;
      pos++;
      break;
    case '(':
      tokens[token_count].token_type = TOKEN_LPAREN;
      strcpy(tokens[token_count].data, "(");
      token_count++;
      pos++;
      break;
    case ')':
      tokens[token_count].token_type = TOKEN_RPAREN;
      strcpy(tokens[token_count].data, ")");
      token_count++;
      pos++;
    }
  }
  return token_count;
}

int get_line(char *data, FILE *fptr) {
  if (!fptr) {
    fprintf(stderr, "Error: NULL file Pointer\n");
    return -1;
  }
  if (!fgets(data, 256, fptr)) {
    if (feof(fptr)) {
      return 0;
    } else {
      return -1;
    }
  }
  return 1;
}
