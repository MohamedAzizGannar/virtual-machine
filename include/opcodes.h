#ifndef OPCODES_H
#define OPCODES_H
#define OP_CODES_COUNT (25);

static const char *OP_CODES[] = {
    "ADD", "SUB", "MUL", "DIV", "AND", "OR",  "XOR", "SHR", "LDB",
    "LDH", "LDW", "STB", "STH", "STD", "JMP", "JZS", "JZC", "JCS",
    "JCC", "JNS", "JNC", "IN",  "OUT", "RND", "HLT"};
#endif
