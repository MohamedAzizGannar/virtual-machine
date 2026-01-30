#ifndef PARSER_H
#define PARSER_H
#include "utils.h"

int parse_instructions(Token *tokens, int token_count,
                       ParsedInstruction *parsed_instruction);

int check_matching_formats(ParsedInstruction *instruction,
                           InstructionFormat fmt);

int validate_instruction(ParsedInstruction *instruction);

#endif // !PARSER_H
