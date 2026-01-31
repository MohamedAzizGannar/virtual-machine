#include "../include/generator.h"
#include <_string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

int get_opcode(char *operation_name) {
  for (int i = 0; i < instruction_table_size; i++) {
    if (strcasecmp(operation_name, instruction_table[i].name) == 0)
      return instruction_table[i].opcode;
  }
  return -1;
}
int get_register_number(char *register_name) {
  int num = atoi(++register_name);
  if (num < 33 && num > -1)
    return num;
  return -1;
}
int get_immediate_value(char *immediate) {
  int base = 10;

  if (*immediate == '#')
    immediate++;
  if (*immediate == '\0') {
    fprintf(stderr, "Empty Immediate Value: Exiting\n");
    return -1;
  }
  if (*immediate == 'h' || *immediate == 'H') {
    base = 16;
    immediate++;
  }
  if (*immediate == '\0') {
    fprintf(stderr, "No Digits after h/H: Exiting\n");
    return -1;
  }
  int errno = 0;
  char *end;
  int val = strtol(immediate, &end, base);

  // Check conversion errors
  if (errno != 0 || *end != '\0') {
    fprintf(stderr, "Invalid Immediate Value: %s\n", immediate);
    return -1;
  }

  // Bounds check (16-bit signed/unsigned range)
  if (val < -32768 || val > 65535) {
    fprintf(stderr, "Immediate Value Out of Bounds: %d\n", val);
    return -1;
  }

  return (int)(val & 0xFFFF);
}
uint32_t pack_instruction(int opcode, int rd, int rn, int imm, int src2) {
  uint32_t res = 0;
  res |= (opcode & 0x1F) << 27;
  res |= (rd & 0x1F) << 22;
  res |= (rn & 0x1F) << 17;
  res |= (imm & 0x1) << 16;
  res |= (src2 & 0xFFFF);
  return res;
}
uint32_t encode_arithmetic_instruction(int opcode,
                                       ParsedInstruction *instruction) {

  int rd = get_register_number(instruction->operands[0].data);
  if (rd < 0 || rd > 32) {

    fprintf(stderr,
            "Failure on Line %d during Generation : Invalid Source Register "
            "Number\n",
            instruction->line_number);
    return -1;
  }
  int rn = get_register_number(instruction->operands[1].data);
  if (rn < 0 || rn > 32) {

    fprintf(stderr,
            "Failure on Line %d during Generation : Invalid Source Register "
            "Number\n",
            instruction->line_number);
    return -1;
  }
  int src2 = -1;
  int imm_bit = -1;
  if (instruction->operands[2].operand_type == OPERAND_REGISTER) {
    imm_bit = 0;
    int rm = get_register_number(instruction->operands[2].data);
    if (rm < 0) {
      fprintf(stderr,
              "Failure on Line %d during Generation : Invalid Auxiliary "
              "Register Number\n",
              instruction->line_number);
      return -1;
    }
    src2 = rm;
  } else if (instruction->operands[2].operand_type == OPERAND_NUMBER) {
    imm_bit = 1;

    src2 = get_immediate_value(instruction->operands[2].data) & 0xFFFF;
  } else {
    fprintf(
        stderr,
        "Failure on Line %d during Generation : Invalid Auxiliary Argument\n",
        instruction->line_number);
    return -1;
  }
  return pack_instruction(opcode, rd, rn, imm_bit, src2);
}
uint32_t encode_load_instruction(int opcode, ParsedInstruction *instruction) {
  int rd = get_register_number(instruction->operands[0].data);
  if (rd < 0 || rd > 32) {

    fprintf(stderr,
            "Failure on Line %d during Generation : Invalid Source Register "
            "Number\n",
            instruction->line_number);
    return -1;
  }
  int rn = get_register_number(instruction->operands[1].data);
  if (rn < 0 || rn > 32) {

    fprintf(stderr,
            "Failure on Line %d during Generation : Invalid Source Register "
            "Number\n",
            instruction->line_number);
    return -1;
  }
  int imm_bit = -1;
  int src2 = -1;
  if (instruction->operands[1].has_offset > 0) {
    if (instruction->operands[1].offset_type == OPERAND_REGISTER) {
      imm_bit = 0;
      int rm = get_register_number(instruction->operands[1].offset);
      if (rm < 0) {
        fprintf(stderr,
                "Failure on Line %d during Generation : Invalid Auxiliary "
                "Register\n",
                instruction->line_number);
        return -1;
      }
      src2 = rm;
    } else if (instruction->operands[1].offset_type == OPERAND_NUMBER) {
      imm_bit = 1;
      src2 = get_immediate_value(instruction->operands[1].offset) & 0xFFFF;
    } else {
      fprintf(
          stderr,
          "Failure on Line %d during Generation : Invalid Auxiliary Source \n",
          instruction->line_number);
      return -1;
    }
  }
  return pack_instruction(opcode, rd, rn, imm_bit, src2);
}
uint32_t encode_store_instruction(int opcode, ParsedInstruction *instruction) {
  int rd = get_register_number(instruction->operands[0].data);

  if (rd < 0 || rd > 32) {

    fprintf(stderr,
            "Failure on Line %d during Generation : Invalid Source Register "
            "Number\n",
            instruction->line_number);
    return -1;
  }
  int rn = get_register_number(instruction->operands[1].data);
  if (rn < 0 || rn > 32) {

    fprintf(stderr,
            "Failure on Line %d during Generation : Invalid Source Register "
            "Number\n",
            instruction->line_number);
    return -1;
  }
  int imm_bit = -1;
  int src2 = -1;
  if (instruction->operands[0].has_offset > 0) {
    if (instruction->operands[0].offset_type == OPERAND_REGISTER) {
      imm_bit = 0;
      int rm = get_register_number(instruction->operands[0].offset);
      if (rm < 0) {
        fprintf(stderr,
                "Failure on Line %d during Generation : Invalid Auxiliary "
                "Register\n",
                instruction->line_number);
        return -1;
      }
      src2 = rm;
    } else if (instruction->operands[0].offset_type == OPERAND_NUMBER) {
      imm_bit = 1;
      src2 = get_immediate_value(instruction->operands[0].offset) & 0xFFFF;
    } else {
      fprintf(
          stderr,
          "Failure on Line %d during Generation : Invalid Auxiliary Source \n",
          instruction->line_number);
      return -1;
    }
  }
  return pack_instruction(opcode, rd, rn, imm_bit, src2);
}

uint32_t encode_jump_instruction(int opcode, ParsedInstruction *instruction) {
  int rd = 0;
  int rn = 0;
  int imm_bit = -1;
  int src2 = -1;
  if (instruction->operands[0].has_offset > 0) {
    if (instruction->operands[0].offset_type == OPERAND_REGISTER) {
      imm_bit = 0;
      int rm = get_register_number(instruction->operands[0].offset);
      if (rm < 0) {
        fprintf(stderr,
                "Failure on Line %d during Generation : Invalid Auxiliary "
                "Register\n",
                instruction->line_number);
        return -1;
      }
      src2 = rm;
    } else if (instruction->operands[0].offset_type == OPERAND_NUMBER) {
      imm_bit = 1;
      src2 = get_immediate_value(instruction->operands[0].offset) & 0xFFFF;
    } else {
      fprintf(
          stderr,
          "Failure on Line %d during Generation : Invalid Auxiliary Source \n",
          instruction->line_number);
      return -1;
    }
  }
  return pack_instruction(opcode, rd, rn, imm_bit, src2);
}

uint32_t encode_io_instruction(int opcode, ParsedInstruction *instruction) {
  int rd = get_register_number(instruction->operands[0].data);

  if (rd < 0 || rd > 32) {

    fprintf(stderr,
            "Failure on Line %d during Generation : Invalid Source Register "
            "Number\n",
            instruction->line_number);
    return -1;
  }
  int rn = 0;
  int imm_bit = 0;
  int src2 = 0;
  return pack_instruction(opcode, rd, rn, imm_bit, src2);
}
uint32_t encode_instruction(ParsedInstruction *instruction) {

  int opcode = get_opcode(instruction->opcode);
  if (opcode < 0 || opcode >= 25) {
    fprintf(stderr, "Unknown Operator Type at Line %d\n",
            instruction->line_number);
    return -1;
  } else if (opcode < 8) {
    // Arithmetic Encoding
    return encode_arithmetic_instruction(opcode, instruction);
  } else if (opcode > 11 || opcode < 15) {
    return encode_load_instruction(opcode, instruction);

    // Load Encoding
  } else if (opcode > 14 || opcode < 18) {
    return encode_store_instruction(opcode, instruction);
    // Store Encoding
  } else if (opcode > 20 || opcode < 28) {
    return encode_jump_instruction(opcode, instruction);
    // Jump Encoding
  } else if (opcode > 27 || opcode < 30) {
    return encode_io_instruction(opcode, instruction);
    // IO encoding
  } else if (opcode == 30) {
    return encode_arithmetic_instruction(opcode, instruction);
    // RND Encoding
  } else if (opcode == 31) {
    return pack_instruction(opcode, 0, 0, 0, 0);
    // hlt encoding
  } else {
    return -1;
  }
}
