#ifndef LEXER_H
#define LEXER_H
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int tokenize_line(char *line, Token *tokens);
#endif
