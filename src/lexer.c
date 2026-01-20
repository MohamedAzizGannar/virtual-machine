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
      } else if (line[start] == 'r' && word_len > 1 &&
                 isnumber(line[start + 1]))
        tokens[token_count].token_type = TOKEN_REGISTER;
      else {
        int is_opcode = 0;
        for (int i = 0; i < OP_CODES_COUNT) {
          if (strcmp(tokens[token_count].data, OP_CODES[i]) == 0) {
            tokens[token_count].token_type = TOKEN_OPCODE;
            is_opcode = 1;
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
// Debug Function
const char *token_type_to_string(TokenType type) {
  switch (type) {
  case TOKEN_OPCODE:
    return "TOKEN_OPCODE";
  case TOKEN_REGISTER:
    return "TOKEN_REGISTER";
  case TOKEN_NUMBER:
    return "TOKEN_NUMBER";
  case TOKEN_LABEL:
    return "TOKEN_LABEL";
  case TOKEN_IDENTIFIER:
    return "TOKEN_IDENTIFIER";
  case TOKEN_COMMA:
    return "TOKEN_COMMA";
  case TOKEN_NEWLINE:
    return "TOKEN_NEWLINE";
  case TOKEN_EOF:
    return "TOKEN_EOF";
  case TOKEN_LPAREN:
    return "TOKEN_LPAREN";
  case TOKEN_RPAREN:
    return "TOKEN_RPAREN";
  case TOKEN_UNKNOWN:
    return "TOKEN_UNKNOWN";
  default:
    return "UNKNOWN_TYPE";
  }
}
