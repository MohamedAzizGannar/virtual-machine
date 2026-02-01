#include "../include/system.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int read_file(char *filename, char ***data) {
  FILE *file_ptr = fopen(filename, "r");
  if (!file_ptr) {
    fprintf(stderr, "Failure Loading File, Please Check Path And Name \n %s \n",
            filename);
    return -1;
  }
  printf("Success Loading File\n");
  int line_count = 0;
  char line_buffer[256];
  while (fgets(line_buffer, sizeof(line_buffer), file_ptr)) {
    line_count++;
  }
  rewind(file_ptr);
  *data = malloc(line_count * sizeof(char *));
  if (!*data) {
    fprintf(stderr, "Data Memory allocation failed\n");
    fclose(file_ptr);
    return -1;
  }
  int i = 0;
  while (fgets(line_buffer, sizeof(line_buffer), file_ptr)) {
    (*data)[i] = strdup(line_buffer);
    if (!(*data)[i]) {
      fprintf(stderr, "Data strdup Failed At Line : %d\n", i);
      for (int j = 0; j < i; j++) {
        free((*data)[j]);
      }
      free(*data);
      fclose(file_ptr);
      return -1;
    }
    i++;
  }
  return line_count;
}
int first_pass(char **data, int line_count, Token **tokens, int *tokens_count,
               SymbolTable *symbol_table) {
  int curr_address = 0;
  for (int i = 0; i < line_count; i++) {
    int token_count = tokenize_line(data[i], tokens[i]);
    if (token_count < 0) {
      fprintf(stderr, "Failure Tokenizing Line %d: Exiting \n", i);
      return -1;
    }
    tokens_count[i] = token_count;
    if (token_count > 0 && tokens[i][0].token_type == TOKEN_LABEL) {
      printf("DEBUG: Adding label '%s' at address %d\n", tokens[i][0].data,
             curr_address);
      strcpy(symbol_table->entries[symbol_table->count].data,
             tokens[i][0].data);
      symbol_table->entries[symbol_table->count++].address = curr_address;
    }
    curr_address += 4;
  }
  return 1;
}
int second_pass(int line_count, Token **tokens, int *token_count,
                ParsedInstruction *parsedInstrucions, SymbolTable *symbol_table,
                int *instruction_count_out) {
  int instruction_count = 0;
  for (int i = 0; i < line_count; i++) {
    if (token_count[i] == 0 ||
        (token_count[i] == 1 && tokens[i][0].token_type == TOKEN_NEWLINE) ||
        (token_count[i] == 1 && tokens[i][0].token_type == TOKEN_LABEL)) {
      continue;
    }
    int parse_success = parse_instructions(
        tokens[i], token_count[i], &parsedInstrucions[instruction_count]);
    if (parse_success < 0) {
      fprintf(stderr, "Failure Parsing Line %d: Exiting\n", i);
      return -1;
    }
    parsedInstrucions[instruction_count].line_number = i;
    int validate_success =
        validate_instruction(&parsedInstrucions[instruction_count]);
    if (validate_success < 0) {
      fprintf(stderr, "Failure Validating Line %d: Exiting \n", i);
      return -1;
    }
    for (int j = 0; j < parsedInstrucions[i].operand_count; j++) {
      if (parsedInstrucions[instruction_count].operands[j].operand_type ==
          OPERAND_IDENTIFIER) {
        int identifier_address =
            lookup_table(symbol_table,
                         parsedInstrucions[instruction_count].operands[j].data);
        if (identifier_address < 0) {
          fprintf(stderr,
                  "No Matching Label found for %s Identifier on Line %d: "
                  "Exiting \n",
                  parsedInstrucions[instruction_count].operands[j].data, i);
          return -1;
        }
        parsedInstrucions[instruction_count].operands[j].operand_type =
            OPERAND_NUMBER;
        sprintf(parsedInstrucions[instruction_count].operands[j].data, "%d",
                identifier_address);
      }
    }
    print_parsed_instruction(&parsedInstrucions[instruction_count]);
    instruction_count++;
  }
  *instruction_count_out = instruction_count;

  return 1;
}

int run_system(char *filename) {
  int line_count = 0;
  char **data = NULL;
  line_count = read_file(filename, &data);

  SymbolTable *symbol_table = malloc(sizeof(SymbolTable));
  if (!symbol_table) {
    fprintf(stderr, "Failure Creating Symbol Table \n");
    return -1;
  }
  symbol_table->count = 0;
  Token **tokens = malloc(line_count * sizeof(Token *));
  if (!tokens) {
    fprintf(stderr, "Failure Creating Tokens\n");
    return -1;
  }
  for (int i = 0; i < line_count; i++) {
    tokens[i] = malloc(256 * sizeof(Token));
    if (!tokens[i]) {
      fprintf(stderr, "Memory allocation failed\n");
      for (int j = 0; j < i; j++) {
        free(tokens[j]);
      }
      free(tokens);
      return -1;
    }
  }
  int tokens_count[line_count];
  ParsedInstruction *parsed_instructions =
      malloc(line_count * sizeof(ParsedInstruction));

  int first_pass_success =
      first_pass(data, line_count, tokens, tokens_count, symbol_table);
  if (first_pass_success < 0) {
    fprintf(stderr, "Failure during the first Pass: Exiting\n");
    return -1;
  }
  printf("First Pass Success -> Starting Second Pass\n");

  int instruction_count = 0;
  int second_pass_sucess =
      second_pass(line_count, tokens, tokens_count, parsed_instructions,
                  symbol_table, &instruction_count);
  if (second_pass_sucess < 0) {
    fprintf(stderr, "Failure during the second Pass: Exiting\n");
    return -1;
  }
  printf("Second Pass Success -> Starting Code Generation\n");

  int generation_success = generate_hexadecimal_file(
      parsed_instructions, instruction_count, "hexa.txt");
  if (generation_success > 0) {
    printf("Generation Success->Starting Simulation\n");
  }
  int simulate_success = simulate("hexa.txt");
  if (simulate_success > 0) {
    printf("Simulation Success\n");
  } else {
    fprintf(stderr, "Failure During Simulation : Exiting\n");
    return -1;
  }

  for (int i = 0; i < line_count; i++) {
    free(data[i]);
  }
  free(data);

  for (int i = 0; i < line_count; i++) {
    free(tokens[i]);
  }
  free(tokens);

  free(parsed_instructions);

  free(symbol_table);
  return 1;
}
