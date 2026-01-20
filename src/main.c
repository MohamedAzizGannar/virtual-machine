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
    int success = parse_instructions(tokens[j], count, &parsed_instructions[j]);
    if (success == -1) {
      printf("Failure Parsing Line %d\n", j);
      return -1;
    } else {
      show_parsed_instruction(&parsed_instructions[j]);
    }
  }
  fclose(fptr);

  return 0;
}
