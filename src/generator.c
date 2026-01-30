#include "../include/generator.h"
#include <_string.h>
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
uint32_t encode_instruction(ParsedInstruction *instruction) {

  int opcode = get_opcode(instruction->opcode);
  OpType op_type = find_op_type(instruction->opcode);
  switch (op_type) {
  case ARITHMETIC:

  case MEMORY:

  case IO:
  case JUMP:
  case MISC:
  case UNKNOWN:
    fprintf(stderr, "Operation Type Not Found: Exiting\n");
    return -1;
    break;
  }
}
