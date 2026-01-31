#ifndef GENERATOR_H
#define GENERATOR_H
#include "utils.h"
#include <stdint.h>

int get_opcode(char *operation_name);
int get_register_number(char *register_name);
int get_immediate_value(char *immediate);
uint32_t encode_instruction(ParsedInstruction *instruction);
uint32_t encode_arithmetic_instruction(int opcode,
                                       ParsedInstruction *instruction);
uint32_t encode_load_instruction(int opcode, ParsedInstruction *instruction);
uint32_t encode_jump_instruction(int opcode, ParsedInstruction *instruction);
int generate_hexadecimal_file(ParsedInstruction **instructions, int count,
                              char *target_filename);
#endif // !GENERATOR_H
