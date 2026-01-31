#include "../include/parser.h"
#include <_strings.h>
#include <stdio.h>
#include <string.h>

int parse_load_instruction(Token *tokens, int token_count, int start_pos,
                           ParsedInstruction *parsed_instruction) {
  int pos = start_pos;
  parsed_instruction->operand_count = 0;
  if (pos < token_count && tokens[pos].token_type == TOKEN_COMMA) {
    pos++;
  }
  if (pos >= token_count) {
    fprintf(stderr, "Expected Token :Exiting \n");
    return -1;
  }
  if (tokens[pos].token_type != TOKEN_REGISTER) {
    fprintf(stderr, "Expected Destination Register, Found %s",
            token_type_to_string(tokens[pos].token_type));
    return -1;
  }
  parsed_instruction->operands[0].operand_type = OPERAND_REGISTER;
  strcpy(parsed_instruction->operands[0].data, tokens[pos].data);
  parsed_instruction->operand_count++;
  pos++;
  if (pos >= token_count) {
    fprintf(stderr, "Expected Comma After Register:Exiting \n");
    return -1;
  }
  if (tokens[pos].token_type != TOKEN_COMMA) {
    fprintf(stderr, "Expected Comma After Register, Found %s",
            token_type_to_string(tokens[pos].token_type));
    return -1;
  }
  pos++;
  if (pos >= token_count) {
    fprintf(stderr, "Expected Memory Register :Exiting \n");
    return -1;
  }
  if (tokens[pos].token_type != TOKEN_LPAREN) {
    fprintf(stderr, "Expected Parentheses for Memory Register, Found %s",
            token_type_to_string(tokens[pos].token_type));
    return -1;
  }
  pos++;
  if (pos >= token_count) {
    fprintf(stderr, "Expected Memory Register After Parentheses:Exiting \n");
    return -1;
  }
  if (tokens[pos].token_type != TOKEN_REGISTER) {
    fprintf(stderr,
            "Expected Memory Register after Opening Parentheses, Found %s",
            token_type_to_string(tokens[pos].token_type));
    return -1;
  }
  parsed_instruction->operands[1].operand_type = OPERAND_MEM;
  strcpy(parsed_instruction->operands[1].data, tokens[pos].data);
  parsed_instruction->operand_count++;
  pos++;
  if (pos >= token_count) {
    fprintf(stderr, "Expected Closing Parentheses After Register :Exiting \n");
    return -1;
  }
  if (tokens[pos].token_type != TOKEN_RPAREN) {
    fprintf(stderr,
            "Expected Closing Parentheses after Memory Register, Found %s",
            token_type_to_string(tokens[pos].token_type));
    return -1;
  }
  pos++;
  if (pos < token_count && tokens[pos].token_type != TOKEN_NEWLINE &&
      tokens[pos].token_type != TOKEN_EOF) {
    if (tokens[pos].token_type == TOKEN_REGISTER) {
      parsed_instruction->operands[1].has_offset = 1;
      strcpy(parsed_instruction->operands[1].offset, tokens[pos].data);
      parsed_instruction->operands[1].operand_type = OPERAND_REGISTER;
    } else if (tokens[pos].token_type == TOKEN_NUMBER) {
      parsed_instruction->operands[1].has_offset = 1;
      strcpy(parsed_instruction->operands[1].offset, tokens[pos].data);
      parsed_instruction->operands[1].operand_type = OPERAND_NUMBER;
    } else if (tokens[pos].token_type == TOKEN_COMMA) {
      printf("Warning, Found Comma After Register\n");
    } else {
      fprintf(stderr, "Unexpected Token After Register of Type : %s",
              token_type_to_string(tokens[pos].token_type));
      return -1;
    }
  } else {
    parsed_instruction->operands[2].has_offset = 0;
  }
  return 1;
}
int parse_store_instruction(Token *tokens, int token_count, int start_pos,
                            ParsedInstruction *parsed_instruction) {

  parsed_instruction->operand_count = 0;
  int pos = start_pos;
  if (pos >= token_count) {
    fprintf(stderr, "Expected '(' for Memory Register :Exiting \n");
    return -1;
  }
  if (tokens[pos].token_type != TOKEN_LPAREN) {
    fprintf(stderr, "Expected Parentheses for Memory Register, Found %s",
            token_type_to_string(tokens[pos].token_type));
    return -1;
  }
  pos++;
  if (pos >= token_count) {
    fprintf(stderr, "Expected Memory Register After Parentheses:Exiting \n");
    return -1;
  }
  if (tokens[pos].token_type != TOKEN_REGISTER) {
    fprintf(stderr,
            "Expected Memory Register after Opening Parentheses, Found %s",
            token_type_to_string(tokens[pos].token_type));
    return -1;
  }
  parsed_instruction->operands[0].operand_type = OPERAND_MEM;
  strcpy(parsed_instruction->operands[0].data, tokens[pos].data);
  parsed_instruction->operand_count++;
  pos++;
  if (pos >= token_count) {
    fprintf(stderr, "Expected Closing Parentheses After Register :Exiting \n");
    return -1;
  }
  if (tokens[pos].token_type != TOKEN_RPAREN) {
    fprintf(stderr,
            "Expected Closing Parentheses after Memory Register, Found %s",
            token_type_to_string(tokens[pos].token_type));
    return -1;
  }
  if (pos < token_count && tokens[pos].token_type != TOKEN_NEWLINE &&
      tokens[pos].token_type != TOKEN_EOF) {
    if (tokens[pos].token_type == TOKEN_REGISTER) {
      parsed_instruction->operands[1].has_offset = 1;
      strcpy(parsed_instruction->operands[1].offset, tokens[pos].data);
      parsed_instruction->operands[1].operand_type = OPERAND_REGISTER;
    } else if (tokens[pos].token_type == TOKEN_NUMBER) {
      parsed_instruction->operands[1].has_offset = 1;
      strcpy(parsed_instruction->operands[1].offset, tokens[pos].data);
      parsed_instruction->operands[1].operand_type = OPERAND_NUMBER;
    } else if (tokens[pos].token_type == TOKEN_COMMA) {
      printf("Warning, Found Comma After Register\n");
    } else {
      fprintf(stderr, "Unexpected Token After Register of Type : %s",
              token_type_to_string(tokens[pos].token_type));
      return -1;
    }
  } else {
    parsed_instruction->operands[2].has_offset = 0;
  }
}
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
  if (strcasecmp(parsed_instruction->opcode, "ldb") == 0 ||
      strcasecmp(parsed_instruction->opcode, "ldh") == 0 ||
      strcasecmp(parsed_instruction->opcode, "ldw") == 0) {
  }
  if (strcasecmp(parsed_instruction->opcode, "stb") == 0 ||
      strcasecmp(parsed_instruction->opcode, "sth") == 0 ||
      strcasecmp(parsed_instruction->opcode, "stw") == 0) {
  }
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
      if (tokens[pos].token_type == TOKEN_NUMBER) {
      }
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
