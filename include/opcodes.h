#ifndef OPCODES_H
#define OPCODES_H
#define OP_CODES_COUNT 25
typedef enum {
  FMT_REG_REG_REG,
  FMT_REG_REG,
  FMT_REG_IMM,
  FMT_LABEL,
  FMT_NO_OPERAND
} InstructionFormat;
typedef struct {
  char *name;
  unsigned char opcode;
  InstructionFormat format;
} Instruction;
static const char *OP_CODES[] = {
    "add", "sub", "mul", "div", "and", "or",  "xor", "shr", "ldb",
    "ldh", "ldw", "stb", "sth", "std", "jmp", "jzs", "jzc", "jcs",
    "jcc", "jns", "jnc", "in",  "out", "rnd", "hlt"};
#endif
