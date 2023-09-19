#ifndef __PARSER_H__
#define __PARSER_H__

typedef enum {
    AST_NUMBER,
    AST_SYMBOL,
    AST_IF,
    AST_LIST
} AST_TYPE;

struct AST;

typedef struct {
    struct AST *condition;
    struct AST *consequence;
    struct AST *alternative;
} AST_IF_STATEMENT;

typedef enum {
  O_PL,
  O_MI,
  O_GT,
  O_LT
} Operator;

typedef struct {
  Operator operator;
  struct AST *left;
  struct AST *right;
} AST_BINARY_EXPRESSION;

typedef struct AST {
    AST_TYPE type;
    union {
        int number;
        char symbol[64];
        AST_IF_STATEMENT if_statement;
        AST_BINARY_EXPRESSION binary_expression;
        struct {
            int length;
            struct AST **items;
        } *list;
    };
} AST;

AST *parse_file(const char *filename);

void print_ast(const AST *ast);

#endif
