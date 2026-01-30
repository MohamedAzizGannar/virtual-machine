#ifndef GENERATOR_H
#define GENERATOR_H
#include "utils.h"
#include <stdint.h>

int get_opcode(char *operation_name);
int get_register_number(char *register_name);
int get_immediate_value(char *immediate);
uint32_t encode_instruction(ParsedInstruction *instruction);
#endif // !GENERATOR_H
