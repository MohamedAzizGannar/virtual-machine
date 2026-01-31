#ifndef UTILS_H
#define UTILS_H
#define OP_CODES_COUNT (25);

typedef enum {
  TOKEN_OPCODE,
  TOKEN_REGISTER,
  TOKEN_NUMBER,
  TOKEN_LABEL,
  TOKEN_IDENTIFIER,
  TOKEN_COMMA,
  TOKEN_NEWLINE,
  TOKEN_EOF,
  TOKEN_LPAREN,
  TOKEN_RPAREN,
  TOKEN_UNKNOWN
} TokenType;

typedef struct Token {
  TokenType token_type;
  char data[256];
} Token;

typedef enum {
  FMT_REG_REG_REG,
  FMT_REG_REG_IMM,
  FMT_REG_MEM,
  FMT_MEM_REG,
  FMT_REG_IMM,
  FMT_LABEL,
  FMT_NO_OPERAND,
  FMT_DEFAULT
} InstructionFormat;

typedef enum {
  OPERAND_REGISTER,
  OPERAND_NUMBER,
  OPERAND_MEM,
  OPERAND_IDENTIFIER
} OperandType;

typedef struct InstructionDefinition {
  char *name;
  unsigned char opcode;
  InstructionFormat possible_formats[4];
  int format_count;
} InstructionDefinition;

typedef struct SymbolEntry {
  int address;
  char data[256];
} SymbolEntry;

typedef struct SymbolTable {
  SymbolEntry *entries[256];
  int count;
} SymbolTable;

typedef struct Operand {
  OperandType operand_type;
  char data[256];
  InstructionFormat format;
  int has_offset;
  char offset[256];
  OperandType offset_type;
} Operand;

typedef struct ParsedInstruction {

  InstructionFormat instruction_format;
  Operand operands[3];
  char opcode[256];
  int operand_count;
  int line_number;

} ParsedInstruction;
typedef enum OpType { ARITHMETIC, MEMORY, JUMP, IO, MISC, UNKNOWN } OpType;

InstructionDefinition instruction_table[] = {
    {"add", 0x01, {FMT_REG_REG_REG, FMT_REG_REG_IMM}, 3},
    {"sub", 0x02, {FMT_REG_REG_REG, FMT_REG_REG_IMM}, 3},
    {"mul", 0x03, {FMT_REG_REG_REG, FMT_REG_REG_IMM}, 2},
    {"div", 0x04, {FMT_REG_REG_REG, FMT_REG_REG_IMM}, 2},
    {"and", 0x05, {FMT_REG_REG_REG, FMT_REG_REG_IMM}, 2},
    {"or", 0x06, {FMT_REG_REG_REG, FMT_REG_REG_IMM}, 2},
    {"xor", 0x07, {FMT_REG_REG_REG, FMT_REG_REG_IMM}, 2},
    {"shr", 0x08, {FMT_REG_REG_IMM}, 1},
    {"ldb", 0x09, {FMT_REG_MEM}, 1},
    {"ldh", 0x0A, {FMT_REG_MEM}, 1},
    {"ldw", 0x0B, {FMT_REG_MEM, FMT_REG_IMM}, 2},
    {"stb", 0x0C, {FMT_MEM_REG}, 1},
    {"sth", 0x0D, {FMT_MEM_REG}, 1},
    {"std", 0x0E, {FMT_MEM_REG}, 1},
    {"jmp", 0x0F, {FMT_LABEL}, 1},
    {"jzs", 0x10, {FMT_LABEL}, 1},
    {"jzc", 0x11, {FMT_LABEL}, 1},
    {"jcs", 0x12, {FMT_LABEL}, 1},
    {"jcc", 0x13, {FMT_LABEL}, 1},
    {"jns", 0x14, {FMT_LABEL}, 1},
    {"jnc", 0x15, {FMT_LABEL}, 1},
    {"in", 0x16, {FMT_REG_IMM}, 1},
    {"out", 0x17, {FMT_REG_IMM}, 1},
    {"rnd", 0x18, {FMT_REG_IMM}, 1},
    {"hlt", 0x19, {FMT_NO_OPERAND}, 1}};

int instruction_table_size =
    sizeof(instruction_table) / sizeof(InstructionDefinition);

const char *operand_type_to_string(OperandType operand);

void print_parsed_instruction(ParsedInstruction *instruction);

const char *format_to_string(InstructionFormat format);

const char *token_type_to_string(TokenType type);

int lookup_table(SymbolTable *symbol_table, char *label);

int table_contains(SymbolTable *symbol_table, char *label);

int check_is_immediate_format(InstructionFormat format);

OpType find_op_type(char *op_name);

static const char *OP_CODES[] = {
    "ADD", "SUB", "MUL", "DIV", "AND", "OR",  "XOR", "SHR", "LDB",
    "LDH", "LDW", "STB", "STH", "STD", "JMP", "JZS", "JZC", "JCS",
    "JCC", "JNS", "JNC", "IN",  "OUT", "RND", "HLT"};

#endif // !UTILS_H
