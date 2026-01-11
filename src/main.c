#include "../include/lexer.h"
#include <stdio.h>
int main(int argc, char *argv[]) {
  FILE *fptr = fopen(argv[1], "r");
  if (!fptr) {
    printf("FAIL");
    return -1;
  }
  char data_arr[256][256];
  Token tokens[256];
  int tokens_len[256];
  int i = 0;
  printf("Success\n");
  while (get_line(data_arr[i], fptr)) {
    i++;
  }
  printf("Get Line Success\n\n");
  int len = i;
  printf("%s", data_arr[0]);
  tokenize_line(data_arr[0], tokens);
  fclose(fptr);

  return 0;
}
