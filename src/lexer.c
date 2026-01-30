#include "../include/lexer.h"
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
      int start = pos;
      while (pos < len && (isalnum(line[pos]) || line[pos] == '_')) {
        pos++;
      }
      int word_len = pos - start;

      strncpy(tokens[token_count].data, &line[start], word_len);
      tokens[token_count].data[word_len] = '\0';

      if (pos < len && (line[pos] == ':' || line[pos + 1] == ':')) {
        tokens[token_count].token_type = TOKEN_LABEL;
        pos++;
      } else if (line[start] == 'r' && word_len > 1 && isdigit(line[start + 1]))
        tokens[token_count].token_type = TOKEN_REGISTER;
      else {
        int is_opcode = 0;
        for (int i = 0; i < OP_CODES_COUNT) {
          if (strcasecmp(tokens[token_count].data, OP_CODES[i]) == 0) {
            tokens[token_count].token_type = TOKEN_OPCODE;
            is_opcode = 1;
            break;
          }
          i++;
        }
        if (!is_opcode)
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
    switch (line[pos]) {
    case ',':
      strcpy(tokens[token_count].data, ",");
      tokens[token_count].token_type = TOKEN_COMMA;
      token_count++;
      pos++;
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
      break;
    case ';':
      pos = len;
      break;
    default:
      pos++;
      break;
    }
  }
  return token_count;
}

// Debug Function
