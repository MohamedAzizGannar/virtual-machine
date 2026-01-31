#include "../include/utils.h"

#include <_strings.h>
#include <stdio.h>
#include <string.h>

const InstructionDefinition instruction_table[] = {
    {"or", 0, {FMT_REG_REG_REG, FMT_REG_REG_IMM}, 2},
    {"and", 1, {FMT_REG_REG_REG, FMT_REG_REG_IMM}, 2},
    {"xor", 2, {FMT_REG_REG_REG, FMT_REG_REG_IMM}, 2},
    {"add", 3, {FMT_REG_REG_REG, FMT_REG_REG_IMM}, 2},
    {"sub", 4, {FMT_REG_REG_REG, FMT_REG_REG_IMM}, 3},
    {"mul", 5, {FMT_REG_REG_REG, FMT_REG_REG_IMM}, 2},
    {"div", 6, {FMT_REG_REG_REG, FMT_REG_REG_IMM}, 2},
    {"shr", 7, {FMT_REG_REG_IMM}, 1},
    {"ldb", 12, {FMT_REG_MEM}, 1},
    {"ldh", 13, {FMT_REG_MEM}, 1},
    {"ldw", 14, {FMT_REG_MEM, FMT_REG_IMM}, 2},
    {"stb", 15, {FMT_MEM_REG}, 1},
    {"sth", 16, {FMT_MEM_REG}, 1},
    {"stw", 17, {FMT_MEM_REG}, 1},
    {"jmp", 21, {FMT_LABEL, FMT_REG}, 2},
    {"jzs", 22, {FMT_LABEL, FMT_REG}, 2},
    {"jzc", 23, {FMT_LABEL, FMT_REG}, 2},
    {"jcs", 24, {FMT_LABEL, FMT_REG}, 2},
    {"jcc", 25, {FMT_LABEL, FMT_REG}, 2},
    {"jns", 26, {FMT_LABEL, FMT_REG}, 2},
    {"jnc", 27, {FMT_LABEL, FMT_REG}, 2},
    {"in", 28, {FMT_REG}, 1},
    {"out", 29, {FMT_REG}, 1},
    {"rnd", 30, {FMT_REG_REG_REG, FMT_REG_REG_IMM}, 2},
    {"hlt", 31, {FMT_NO_OPERAND}, 1}};

const int instruction_table_size =
    sizeof(instruction_table) / sizeof(InstructionDefinition);

int table_contains(SymbolTable *symbol_table, char *label) {
  for (int i = 0; i < symbol_table->count; i++) {
    if (strcmp(symbol_table->entries[i].data, label) == 0) {
      return 1;
    }
  }
  return -1;
}
int lookup_table(SymbolTable *symbol_table, char *label) {
  for (int i = 0; i < symbol_table->count; i++) {
    if (strcasecmp(symbol_table->entries[i].data, label) == 0) {
      return symbol_table->entries[i].address;
    }
  }
  return -1;
}
int check_is_immediate_format(InstructionFormat format) {
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
    if (instruction->operands[i].has_offset) {
      printf("OFFSET - %s\n", instruction->operands[i].offset);
    }
  }
  printf("-----------------------------------\n");
}
