#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdio.h>
#include "lexer.h"

typedef enum {
  AST_UNDEFINED,
  AST_NUMBER,
  AST_SYMBOL,
  AST_IF,
	AST_BINARY,

  AST_STMT_EXP,
  AST_STMT_ASN,
  AST_STMT_SEQ
} AST_TYPE;

struct AST;

typedef struct {
    struct AST *condition;
    struct AST *consequence;
    struct AST *alternative;
} AST_IF_STATEMENT;

typedef enum {
  O_UNDEFINED,
  O_PL,
  O_MI,
  O_MU,
  O_DI,
  O_EQ,
  O_NE,
  O_GT,
  O_LT
} Operator;

typedef struct {
  Operator operator;
  struct AST *left;
  struct AST *right;
} AST_BINARY_EXPRESSION;

typedef struct {
  char identifier[64];
  struct AST *expression;
} AST_ASSIGNMENT;

typedef struct AST_SEQUENCE {
  struct AST *statement;
  struct AST_SEQUENCE *next;
} AST_SEQUENCE;

typedef struct AST {
    AST_TYPE type;
    token *first_token;
    token *last_token;
    union {
      long number;
      char symbol[64];
      AST_IF_STATEMENT if_statement;
      AST_BINARY_EXPRESSION binary_expression;
      AST_ASSIGNMENT assignment;
      struct AST *expression;
      AST_SEQUENCE *sequence;
    };
} AST;

AST *parse_file(FILE *file);

int ast_destroy(AST **ast);

void print_ast(const AST *ast);

#endif
