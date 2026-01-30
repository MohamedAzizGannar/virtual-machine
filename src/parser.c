#include "../include/parser.h"
#include <stdio.h>
#include <string.h>

int parse_instructions(Token *tokens, int token_count,
                       ParsedInstruction *parsed_instruction) {
  int pos = 0;
  parsed_instruction->operand_count = 0;
  parsed_instruction->instruction_format = FMT_DEFAULT;

  if (token_count > 0 && (tokens[pos].token_type == TOKEN_LABEL ||
                          tokens[pos].token_type == TOKEN_IDENTIFIER)) {
    pos++;
  }
  if (tokens[pos].token_type != TOKEN_OPCODE) {
    fprintf(stderr, "Expected Token OPCODE, found %s\n",
            token_type_to_string(tokens[pos].token_type));
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
        return -1;
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
    return -1;
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
  int valid = -1;
  for (int i = 0; i < def->format_count; i++) {
    if (check_matching_formats(instruction, def->possible_formats[i])) {
      valid = 1;
      break;
    }
  }
  if (valid < 0) {
    fprintf(stderr, "Line %d: Invalid Operands for %s Operator\n",
            instruction->line_number, instruction->opcode);
    return -1;
  }
  return 1;
}
