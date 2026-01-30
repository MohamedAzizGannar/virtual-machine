#include "../include/utils.h"

#include <stdio.h>
#include <string.h>

int table_contains(SymbolTable *symbol_table, char *label) {
  for (int i = 0; i < symbol_table->count; i++) {
    if (strcmp(symbol_table->entries[i]->data, label) == 0) {
      return 1;
    }
  }
  return -1;
}
int lookup_table(SymbolTable *symbol_table, char *label) {
  for (int i = 0; i < symbol_table->count; i++) {
    if (strcmp(symbol_table->entries[i]->data, label) == 0) {
      return symbol_table->entries[i]->address;
    }
  }
  return -1;
}
int check_if_is_immediate_format(InstructionFormat format) {
  if (format == FMT_REG_REG_IMM || format == FMT_REG_IMM)
    return 1;
  else
    return -1;
}
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
const char *operand_type_to_string(OperandType operand) {
  switch (operand) {
  case OPERAND_MEM:
    return "Memory Operand";
    break;
  case OPERAND_NUMBER:
    return "Number Operand";
    break;
  case OPERAND_REGISTER:
    return "Register Operand";
    break;
  case OPERAND_IDENTIFIER:
    return "Label Identifier Operand";
    break;
  default:
    return "Unknown Operand Type";
    break;
  }
}
const char *format_to_string(InstructionFormat format) {
  switch (format) {
  case FMT_REG_REG_REG:
    return "REG_REG_REG";
  case FMT_REG_REG_IMM:
    return "REG_REG_IMM";
  case FMT_REG_MEM:
    return "REG_MEM";
  case FMT_MEM_REG:
    return "MEM_REG";
  case FMT_REG_IMM:
    return "REG_IMM";
  case FMT_LABEL:
    return "LABEL";
  case FMT_NO_OPERAND:
    return "NO_OPERAND";
  default:
    return "UNKNOWN";
  }
}

void print_parsed_instruction(ParsedInstruction *instruction) {
  printf("\nParsed Instruction (Line %d)\n", instruction->line_number);
  printf("Opcode: %s\n", instruction->opcode);
  printf("Format: %s\n", format_to_string(instruction->instruction_format));
  printf("Operand Count: %d\n", instruction->operand_count);

  for (int i = 0; i < instruction->operand_count; i++) {
    printf("  Operand %d: ", i + 1);
    switch (instruction->operands[i].operand_type) {
    case OPERAND_REGISTER:
      printf("REGISTER - %s\n", instruction->operands[i].data);
      break;
    case OPERAND_NUMBER:
      printf("NUMBER - %s\n", instruction->operands[i].data);
      break;
    case OPERAND_MEM:
      printf("MEMORY - (%s)\n", instruction->operands[i].data);
      break;
    case OPERAND_IDENTIFIER:
      printf("IDENTIFIER - %s\n", instruction->operands[i].data);
      break;
    default:
      printf("UNKNOWN\n");
    }
  }
  printf("-----------------------------------\n");
}
