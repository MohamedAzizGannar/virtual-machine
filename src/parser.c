#include "../include/parser.h"
#include <stdio.h>

InstructionDefinition instruction_table[] = {
    {"add", 0x01, {FMT_REG_REG_REG, FMT_REG_REG_IMM, FMT_REG_IMM_IMM}, 3},
    {"sub", 0x02, {FMT_REG_REG_REG, FMT_REG_REG_IMM, FMT_REG_IMM_IMM}, 3},
    {"mul", 0x03, {FMT_REG_REG_REG, FMT_REG_REG_IMM}, 2},
    {"div", 0x04, {FMT_REG_REG_REG, FMT_REG_REG_IMM}, 2},
    {"and", 0x05, {FMT_REG_REG_REG, FMT_REG_REG_IMM}, 2},
    {"or", 0x06, {FMT_REG_REG_REG, FMT_REG_REG_IMM}, 2},
    {"xor", 0x07, {FMT_REG_REG_REG, FMT_REG_REG_IMM}, 2},
    {"shr", 0x08, {FMT_REG_REG_IMM}, 1},
    {"ldb", 0x09, {FMT_REG_MEM}, 1},
    {"ldh", 0x0A, {FMT_REG_MEM}, 1},
    {"ldw", 0x0B, {FMT_REG_MEM, FMT_REG_IMM}, 2},
    {"stb", 0x0C, {FMT_MEM_REG}, 1},
    {"sth", 0x0D, {FMT_MEM_REG}, 1},
    {"std", 0x0E, {FMT_MEM_REG}, 1},
    {"jmp", 0x0F, {FMT_LABEL}, 1},
    {"jzs", 0x10, {FMT_LABEL}, 1},
    {"jzc", 0x11, {FMT_LABEL}, 1},
    {"jcs", 0x12, {FMT_LABEL}, 1},
    {"jcc", 0x13, {FMT_LABEL}, 1},
    {"jns", 0x14, {FMT_LABEL}, 1},
    {"jnc", 0x15, {FMT_LABEL}, 1},
    {"in", 0x16, {FMT_REG_IMM}, 1},
    {"out", 0x17, {FMT_REG_IMM}, 1},
    {"rnd", 0x18, {FMT_REG_IMM}, 1},
    {"hlt", 0x19, {FMT_NO_OPERAND}, 1}};
int instruction_table_size =
    sizeof(instruction_table) / sizeof(InstructionDefinition);
int parse_instructions(Token *tokens, int token_count,
                       ParsedInstruction *parsed_instruction) {
  int pos = 0;
  parsed_instruction->operand_count = 0;

  if (token_count > 0 && (tokens[pos].token_type == TOKEN_LABEL ||
                          tokens[pos].token_type == TOKEN_IDENTIFIER)) {
    pos++;
  }
  if (tokens[pos].token_type != TOKEN_OPCODE) {
    fprintf(stderr, "Expected Token OPCODE\n");
    return -1;
  }
  strcpy(parsed_instruction->opcode, tokens[pos].data);
  pos++;
  while (pos < token_count && tokens[pos].token_type != TOKEN_NEWLINE) {
    if (tokens[pos].token_type == TOKEN_COMMA) {
      pos++;
      continue;
    }
    if (tokens[pos].token_type == TOKEN_LPAREN) {
      pos++;
      if (pos >= token_count || tokens[pos].token_type != TOKEN_REGISTER) {
        fprintf(stderr, "Expected Register in Parentheses, Found : %s\n",
                token_type_to_string(tokens[pos].token_type));
      }
      parsed_instruction->operands[parsed_instruction->operand_count]
          .operand_type = OPERAND_MEM;
      strcpy(
          parsed_instruction->operands[parsed_instruction->operand_count].data,
          tokens[pos].data);
      pos++;
      if (tokens[pos].token_type != TOKEN_RPAREN) {
        fprintf(stderr, "Expected Closing Parentheses, Found : %s\n",
                token_type_to_string(tokens[pos].token_type));
        return -1;
      }
      pos++;
      parsed_instruction->operand_count++;
    } else if (tokens[pos].token_type == TOKEN_REGISTER) {
      strcpy(
          parsed_instruction->operands[parsed_instruction->operand_count].data,
          tokens[pos].data);
      parsed_instruction->operands[parsed_instruction->operand_count]
          .operand_type = OPERAND_REGISTER;
      pos++;
      parsed_instruction->operand_count++;
    } else if (tokens[pos].token_type == TOKEN_NUMBER) {
      strcpy(
          parsed_instruction->operands[parsed_instruction->operand_count].data,
          tokens[pos].data);
      parsed_instruction->operands[parsed_instruction->operand_count]
          .operand_type = OPERAND_NUMBER;
      pos++;
      parsed_instruction->operand_count++;
    } else if (tokens[pos].token_type == TOKEN_IDENTIFIER) {
      strcpy(
          parsed_instruction->operands[parsed_instruction->operand_count].data,
          tokens[pos].data);
      parsed_instruction->operands[parsed_instruction->operand_count]
          .operand_type = OPERAND_IDENTIFIER;
      pos++;
      parsed_instruction->operand_count++;
    } else {
      fprintf(stderr, "Error : Unexpected token type\n");
      return -1;
    }
  }
  return 1;
};
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
void show_parsed_instruction(ParsedInstruction *parsed_instruction) {
  printf(
      "OPCODE : %s OP1(Type) : %s(%s) OP2(Type) : %s(%s) OP3(Type): %s(%s)\n",
      parsed_instruction->opcode, parsed_instruction->operands[0].data,
      operand_type_to_string(parsed_instruction->operands[0].operand_type),
      parsed_instruction->operands[1].data,
      operand_type_to_string(parsed_instruction->operands[1].operand_type),
      parsed_instruction->operands[2].data,
      operand_type_to_string(parsed_instruction->operands[2].operand_type));
}
int check_matching_formats(ParsedInstruction *instruction,
                           InstructionFormat fmt) {
  switch (fmt) {
  case FMT_REG_REG_REG:
    return instruction->operand_count == 3 &&
           instruction->operands[0].operand_type == OPERAND_REGISTER &&
           instruction->operands[1].operand_type == OPERAND_REGISTER &&
           instruction->operands[2].operand_type == OPERAND_REGISTER;

  case FMT_REG_REG_IMM:
    return instruction->operand_count == 3 &&
           instruction->operands[0].operand_type == OPERAND_REGISTER &&
           instruction->operands[1].operand_type == OPERAND_REGISTER &&
           instruction->operands[2].operand_type == OPERAND_NUMBER;
  case FMT_REG_IMM_IMM:
    return instruction->operand_count == 3 &&
           instruction->operands[0].operand_type == OPERAND_REGISTER &&
           instruction->operands[1].operand_type == OPERAND_NUMBER &&
           instruction->operands[2].operand_type == OPERAND_NUMBER;
  case FMT_REG_MEM:
    return instruction->operand_count == 2 &&
           instruction->operands[0].operand_type == OPERAND_REGISTER &&
           instruction->operands[1].operand_type == OPERAND_MEM;
  case FMT_MEM_REG:
    return instruction->operand_count == 2 &&
           instruction->operands[0].operand_type == OPERAND_MEM &&
           instruction->operands[1].operand_type == OPERAND_REGISTER;
  case FMT_REG_IMM:
    return instruction->operand_count == 2 &&
           instruction->operands[0].operand_type == OPERAND_REGISTER &&
           instruction->operands[1].operand_type == OPERAND_NUMBER;
  case FMT_LABEL:
    return instruction->operand_count == 1 &&
           instruction->operands[0].operand_type == OPERAND_IDENTIFIER;
  case FMT_NO_OPERAND:
    return instruction->operand_count == 0;
  default:
    return 0;
  }
}
int validate_instruction(ParsedInstruction *instruction) {
  InstructionDefinition *def = NULL;

  for (int i = 0; i < instruction_table_size; i++) {
    if (strcasecmp(instruction_table[i].name, instruction->opcode) == 0) {
      def = &instruction_table[i];
      break;
    }
  }
  if (def == NULL) {
    fprintf(stderr, "No Matching Format Found For Line %d\n",
            instruction->line_number);
    return -1;
  }
  int valid = 0;
  for (int i = 0; i < def->format_count; i++) {
    if (check_matching_formats(instruction, def->possible_formats[i])) {
      valid = 1;
      break;
    }
  }
  if (!valid) {
    fprintf(stderr, "Line %d: Invalide Operands for %s Operator\n",
            instruction->line_number, instruction->opcode);
    return -1;
  }
  return 1;
}
