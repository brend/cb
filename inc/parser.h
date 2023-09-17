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

typedef struct AST {
    AST_TYPE type;
    union {
        int number;
        char symbol[64];
        AST_IF_STATEMENT if_statement;
        struct {
            int length;
            struct AST **items;
        } *list;
    };
} AST;

AST *parse_file(const char *filename);

void print_ast(const AST *ast);

#endif