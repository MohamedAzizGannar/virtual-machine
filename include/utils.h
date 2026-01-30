#ifndef UTILS_H
#define UTILS_H
#define OP_CODES_COUNT (25);

typedef struct SymbolEntry {
  int address;
  char data[256];
} SymbolEntry;

typedef struct SymbolTable {
  SymbolEntry *entries[256];
  int count;
} SymbolTable;

int lookup_table(SymbolTable *symbol_table, char *label);
int table_contains(SymbolTable *symbol_table, char *label);

static const char *OP_CODES[] = {
    "ADD", "SUB", "MUL", "DIV", "AND", "OR",  "XOR", "SHR", "LDB",
    "LDH", "LDW", "STB", "STH", "STD", "JMP", "JZS", "JZC", "JCS",
    "JCC", "JNS", "JNC", "IN",  "OUT", "RND", "HLT"};

#endif // !UTILS_H
