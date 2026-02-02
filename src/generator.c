#include "../include/generator.h"
#include <_string.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
int get_opcode(char *operation_name) {
  for (int i = 0; i < instruction_table_size; i++) {
    if (strcasecmp(operation_name, instruction_table[i].name) == 0)
      return instruction_table[i].opcode;
  }
  return -1;
}
int get_register_number(char *register_name, int *out) {
  if (!register_name)
    return -1;
  if (register_name[0] != 'r' && register_name[0] != 'R')
    return -1;
  if (register_name[1] == '\0')
    return -1;

  char *end;
  long num = strtol(register_name + 1, &end, 10);

  if (*end != '\0')
    return -1;
  if (num < 0 || num > 31)
    return -1;

  *out = (int)num;
  return 1;
}
int get_immediate_value(char *immediate, int *out) {
  if (!immediate) {
    return -1;
  }
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
  errno = 0;
  char *end;
  int val = strtol(immediate, &end, base);

  if (errno != 0 || end == immediate || *end != '\0') {
    fprintf(stderr, "Invalid immediate value: %s\n", immediate);
    return -1;
  }
  if (val < -32768 || val > 65535) {
    fprintf(stderr, "Immediate Value Out of Bounds: %d\n", val);
    return -1;
  }

  *out = (int)(val & 0xFFFF);
  return 1;
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

  int rd;
  int rd_success = get_register_number(instruction->operands[0].data, &rd);
  if (rd_success < 0) {
    return -1;
  }
  if (rd < 0 || rd > 31) {

    fprintf(stderr,
            "Failure on Line %d during Generation : Invalid Source Register "
            "Number\n",
            instruction->line_number);
    return -1;
  }
  int rn;
  int rn_success = get_register_number(instruction->operands[1].data, &rn);
  if (rn_success < 0) {
    return -1;
  }
  if (rn < 0 || rn > 31) {

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
    int rm;
    int rm_success = get_register_number(instruction->operands[2].data, &rm);
    if (rm_success < 0) {
      fprintf(stderr,
              "Failure on Line %d during Generation : Invalid Auxiliary "
              "Register Number\n",
              instruction->line_number);
      return -1;
    }
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

    int src2_success =
        get_immediate_value(instruction->operands[2].data, &src2) & 0xFFFF;
    if (src2_success < 0) {
      return -1;
    }

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
  int rd;
  int rd_success = get_register_number(instruction->operands[0].data, &rd);
  if (rd_success < 0) {
    return -1;
  }
  if (rd < 0 || rd > 31) {

    fprintf(stderr,
            "Failure on Line %d during Generation : Invalid Source Register "
            "Number\n",
            instruction->line_number);
    return -1;
  }
  int rn;
  int rn_success = get_register_number(instruction->operands[1].data, &rn);
  if (rn_success < 0) {
    return -1;
  }
  if (rn < 0 || rn > 31) {

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
      int rm;
      int rm_success =
          get_register_number(instruction->operands[0].offset, &rm);
      if (rm_success < 0) {
        return -1;
      }
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
      int src2_success =
          get_immediate_value(instruction->operands[1].offset, &src2) & 0xFFFF;

      if (src2_success < 0) {
        return -1;
      }
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

  int rd;
  int rd_success = get_register_number(instruction->operands[0].data, &rd);
  if (rd_success < 0) {
    return -1;
  }
  if (rd < 0 || rd > 31) {

    fprintf(stderr,
            "Failure on Line %d during Generation : Invalid Source Register "
            "Number\n",
            instruction->line_number);
    return -1;
  }
  int rn;
  int rn_success = get_register_number(instruction->operands[1].data, &rn);
  if (rn_success < 0) {
    return -1;
  }
  if (rn < 0 || rn > 31) {

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
      int rm;
      int rm_success =
          get_register_number(instruction->operands[0].offset, &rm);
      if (rm_success < 0) {
        return -1;
      }
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
      int src2_success =
          get_immediate_value(instruction->operands[0].offset, &src2) & 0xFFFF;
      if (src2_success < 0) {
        return -1;
      }
    } else {
      fprintf(stderr,
              "Failure on Line %d during Generation : Invalid Auxiliary "
              "Source \n",
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
  if (instruction->operands[0].operand_type == OPERAND_NUMBER) {
    imm_bit = 1;
    int address;
    int address_success =
        get_immediate_value(instruction->operands[0].data, &address);
    if (address_success < 0) {
      return -1;
    }
    if (address < 0) {
      fprintf(stderr, "Negative Label Address\n");
      return -1;
    } else if (address > 65535) {
      imm_bit = 0;
      fprintf(stderr, "Label Address Out Of Bounds\n");
      return -1;
    }
    src2 = address & 0xFFFF;
  } else if (instruction->operands[0].operand_type == OPERAND_REGISTER) {
    imm_bit = 0;
    int address;
    int address_success =
        get_immediate_value(instruction->operands[0].data, &address);
    if (address_success < 0) {
      return -1;
    }
    if (address < 0) {
      fprintf(stderr, "Negative Label Address\n");
      return -1;
    } else if (address > 65535) {
      imm_bit = 0;
      fprintf(stderr, "Label Address Out Of Bounds\n");
      return -1;
    }
    src2 = address & 0xFFFF;
  }
  return pack_instruction(opcode, rd, rn, imm_bit, src2);
}

uint32_t encode_io_instruction(int opcode, ParsedInstruction *instruction) {
  int rd;
  int rd_success = get_register_number(instruction->operands[0].data, &rd);
  if (rd_success < 0) {
    return -1;
  }

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
  if (opcode < 0 || opcode >= 32) {
    fprintf(stderr, "Unknown Operator Type at Line %d\n",
            instruction->line_number);
    return -1;
  } else if (opcode < 8) {
    return encode_arithmetic_instruction(opcode, instruction);
  } else if (opcode > 11 && opcode < 15) {
    return encode_load_instruction(opcode, instruction);

  } else if (opcode > 14 && opcode < 18) {
    return encode_store_instruction(opcode, instruction);
  } else if (opcode > 20 && opcode < 28) {
    return encode_jump_instruction(opcode, instruction);
  } else if (opcode > 27 && opcode < 30) {
    return encode_io_instruction(opcode, instruction);
  } else if (opcode == 30) {
    return encode_arithmetic_instruction(opcode, instruction);
  } else if (opcode == 31) {
    return pack_instruction(opcode, 0, 0, 0, 0);
  } else {
    return -1;
  }
}
int generate_hexadecimal_file(ParsedInstruction *instructions, int count,
                              char *target_filename) {
  FILE *file_ptr = fopen(target_filename, "w");
  if (!file_ptr) {
    fprintf(stderr, "Failure Creating Target File \n");
    return -1;
  }
  for (int i = 0; i < count; i++) {
    uint32_t encoded = encode_instruction(&instructions[i]);

    if (encoded < 0) {
      fprintf(stderr, "Failure Encoding at Line %d\n",
              instructions[i].line_number);
      fclose(file_ptr);
      return -1;
    }

    fprintf(file_ptr, "%02X%02X%02X%02X\n", (encoded >> 24) & 0xFF,
            (encoded >> 16) & 0xFF, (encoded >> 8) & 0xFF, encoded & 0xFF);
  }
  fclose(file_ptr);
  return 1;
}
