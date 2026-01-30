#include "../include/system.h"
#include <_string.h>
#include <stdio.h>

int read_file(char *filename, char **data, int *line_count) {
  FILE *file_ptr = fopen(filename, "r");
  if (!file_ptr) {
    fprintf(stderr, "Failure Loading File, Please Check Path And Name \n %s \n",
            filename);
    return -1;
  }
  printf("Succes Loading File\n");
  char line_buffer[256];
  while (fgets(line_buffer, sizeof(line_buffer), file_ptr)) {
    *line_count++;
  }
  rewind(file_ptr);
  data = malloc(*line_count * sizeof(char *));
  int i = 0;
  while (fgets(line_buffer, sizeof(line_buffer), file_ptr)) {
    data[i] = strdup(line_buffer);
    i++;
  }
  return 1;
}
int first_pass(char **data, int line_count, Token **tokens, int *tokens_count,
               SymbolTable *symbol_table) {
  int curr_address = 0;
  for (int i = 0; i < line_count; i++) {
    int token_count = tokenize_line(data[i], tokens[i]);
    if (!token_count) {
      fprintf(stderr, "Failure Tokenizing Line %d: Exiting \n", i);
      return -1;
    }
    if (token_count > 0 && tokens[i][0].token_type == TOKEN_LABEL) {
      SymbolEntry *newEntry = malloc(sizeof(SymbolEntry));

      newEntry->address = curr_address;
      strcpy(newEntry->data, tokens[i][0].data);

      symbol_table->entries[symbol_table->count++] = newEntry;
    }
    curr_address += 4;
  }
  return 1;
}
int second_pass(char **data, int line_count, Token **tokens, int *token_count,
                ParsedInstruction *parsedInstrucions,
                SymbolTable *symbol_table) {
  int curr_address = 0;
  for (int i = 0; i < line_count; i++) {
    int parse_success =
        parse_instructions(tokens[i], token_count[i], &parsedInstrucions[i]);
    if (!parse_success) {
      fprintf(stderr, "Failure Parsing Line %d: Exiting\n", i);
      return -1;
    }
    int validate_success = validate_instruction(&parsedInstrucions[i]);
    if (!validate_success) {
      fprintf(stderr, "Failure Validating Line %d: Exiting \n", i);
      return -1;
    }
    for (int j = 0; j < parsedInstrucions[i].operand_count; j++) {
      if (parsedInstrucions[i].operands[j].operand_type == OPERAND_IDENTIFIER) {
        int identifier_address =
            lookup_table(symbol_table, parsedInstrucions[i].operands[j].data);
        if (identifier_address < 0) {
          fprintf(stderr,
                  "No Matching Label found for %s Identifier on Line %d: "
                  "Exiting \n",
                  parsedInstrucions[i].operands[j].data, i);
          return -1;
        }
        parsedInstrucions[i].operands[j].operand_type = OPERAND_NUMBER;
        sprintf(parsedInstrucions[i].operands[j].data, "%d",
                identifier_address);
      }
    }
  }

  return 1;
}

int run_system(char *filename) {
  int line_count = 0;
  char **data;
  read_file(filename, data, &line_count);

  SymbolTable *symbol_table = malloc(sizeof(SymbolTable));
  Token **tokens = malloc(line_count * sizeof(Token *));
  int tokens_count[line_count];
  ParsedInstruction *parsed_instructions =
      malloc(line_count * sizeof(ParsedInstruction));

  int first_pass_success =
      first_pass(data, line_count, tokens, tokens_count, symbol_table);
  if (first_pass_success < 0) {
    fprintf(stderr, "Failure during the first Pass: Exiting \n");
    return -1;
  }
  printf("First Pass Success\n Starting Second Pass\n");

  int second_pass_sucess = second_pass(data, line_count, tokens, tokens_count,
                                       parsed_instructions, symbol_table);
  if (second_pass_sucess < 0) {
    fprintf(stderr, "Failure during the second Pass: Exiting\n");
    return -1;
  }
  printf("Second Pass Success\n Starting Code Generation\n");

  return 1;
}
