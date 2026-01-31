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
extern const InstructionDefinition instruction_table[];
extern const int instruction_table_size;

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
