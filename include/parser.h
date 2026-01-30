#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"

#include "opcodes.h"
typedef enum {
  FMT_REG_REG_REG,
  FMT_REG_REG_IMM,
  FMT_REG_IMM_IMM,
  FMT_REG_MEM,
  FMT_MEM_REG,
  FMT_REG_IMM,
  FMT_LABEL,
  FMT_NO_OPERAND,
  FMT_DEFAULT
} InstructionFormat;

typedef struct InstructionDefinition {
  char *name;
  unsigned char opcode;
  InstructionFormat possible_formats[4];
  int format_count;
} InstructionDefinition;

typedef struct LabelEntry {
  int address;
  char data[256];
} LabelEntry;

typedef enum {
  OPERAND_REGISTER,
  OPERAND_NUMBER,
  OPERAND_MEM,
  OPERAND_IDENTIFIER
} OperandType;

typedef struct Operand {
  OperandType operand_type;
  char data[256];
  InstructionFormat format;
} Operand;

typedef struct ParsedInstruction {
  InstructionFormat instruction_format;
  Operand operands[3];
  char opcode[256];
  int operand_count;
  int line_number;
} ParsedInstruction;

int parse_instructions(Token *tokens, int token_count,
                       ParsedInstruction *parsed_instruction);

const char *operand_type_to_string(OperandType operand);

int check_matching_formats(ParsedInstruction *instruction,
                           InstructionFormat fmt);

int validate_instruction(ParsedInstruction *instruction);

void print_parsed_instruction(ParsedInstruction *instruction);

const char *format_to_string(InstructionFormat format);

#endif // !PARSER_H
