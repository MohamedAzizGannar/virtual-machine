#ifndef OPCODES_H
#define OPCODES_H
#define OP_CODES_COUNT (25);
typedef enum {
  FMT_REG_REG_REG,
  FMT_REG_REG,
  FMT_REG_IMM,
  FMT_LABEL,
  FMT_NO_OPERAND
} InstructionFormat;
typedef struct {
  char *name;
  unsigned char op_code;
  InstructionFormat format;
} Instruction;
static const char *OP_CODES[] = {
    "ADD", "SUB", "MUL", "DIV", "AND", "OR",  "XOR", "SHR", "LDB",
    "LDH", "LDW", "STB", "STH", "STD", "JMP", "JZS", "JZC", "JCS",
    "JCC", "JNS", "JNC", "IN",  "OUT", "RND", "HLT"};
#endif
