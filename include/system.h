#ifndef SYSTEM_H
#define SYSTEM_H
#include "lexer.h"
#include "parser.h"
#include "utils.h"

int run_system(char *filename);

int read_file(char *filename, char ***data);

int first_pass(char **data, int line_count, Token **tokens, int *tokens_count,
               SymbolTable *symbol_table);
int second_pass(int line_count, Token **tokens, int *tokens_count,
                ParsedInstruction *parsedInstrucions,
                SymbolTable *symbol_table);

#endif
