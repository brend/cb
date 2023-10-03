#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdio.h>
#include "lexer.h"
#include "ast.h"

AST *parse_file(FILE *file);

int ast_destroy(AST **ast);

void print_ast(const AST *ast);

#endif
