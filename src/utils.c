#include "../include/utils.h"

#include <string.h>
int table_contains(SymbolTable *symbol_table, char *label) {
  for (int i = 0; i < symbol_table->count; i++) {
    if (strcmp(symbol_table->entries[i]->data, label) == 0) {
      return 1;
    }
  }
  return -1;
}
int lookup_table(SymbolTable *symbol_table, char *label) {
  for (int i = 0; i < symbol_table->count; i++) {
    if (strcmp(symbol_table->entries[i]->data, label) == 0) {
      return symbol_table->entries[i]->address;
    }
  }
  return -1;
}
