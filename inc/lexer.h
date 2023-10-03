#ifndef __LEXER_H__
#define __LEXER_H__

#include <stdio.h>
#include "stream.h"
#include "ast.h"

int tvalid(Token);

Lexer *lexer_open_file(const char *filename);
Lexer *lexer_from_file(FILE *file);
Lexer *lexer_from_expression(const char *expression);

int lexer_destroy(Lexer**);

Token lexer_peek(Lexer*);
Token lexer_pop(Lexer*);

#endif
