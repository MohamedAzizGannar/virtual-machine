#include "../include//parser.h"
#include "../include/lexer.h"
#include <stdio.h>
int main(int argc, char *argv[]) {
  FILE *fptr = fopen(argv[1], "r");
  if (!fptr) {
    printf("FAIL");
    return -1;
  }
  char data[256][256];
  static Token tokens[256][256];
  static ParsedInstruction parsed_instructions[256];
  int tokens_len[256];
  int i = 0;
  while (get_line(data[i], fptr)) {
    i++;
  }
  printf("Get Line Success\n\n");
  int len = i;
  for (int j = 0; j < len; j++) {
    int count = tokenize_line(data[j], tokens[j]);
    tokens_len[j] = count;
  }
  for (int j = 0; j < len; j++) {
    int success =
        parse_instructions(tokens[j], tokens_len[j], parsed_instructions);
    parsed_instructions[j].line_number = j;
  }
  for (int j = 0; j < len; j++) {
    int valid = validate_instruction(&parsed_instructions[j]);
    if (valid == -1) {
      printf("Line : %s\n", tokens[j][0].data);
      printf("Error Parsing Line %d \n", parsed_instructions[j].line_number);
    }
  }
  fclose(fptr);

  return 0;
}
