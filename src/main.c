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
  int tokens_len[256];
  int i = 0;
  printf("Success\n");
  while (get_line(data[i], fptr)) {
    i++;
  }
  printf("Get Line Success\n\n");
  int len = i;
  for (int j = 0; j < len; j++) {
    int count = tokenize_line(data[j], tokens[j]);
    for (int k = 0; k < count; k++) {
      printf("%s ", tokens[j][k].data);
    }
  }
  fclose(fptr);

  return 0;
}
