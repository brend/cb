#include "parser.h"
#include "lexer.h"
#include <stdlib.h>
#include <string.h>

#define log_parserr(tk, ...) {fprintf(stderr, "error at %d:%d \"%s\": ", (tk).line, (tk).column, (tk).text);fprintf(stderr, __VA_ARGS__);}

AST *parse_expression(lexer *lexer) {
    token t = lexer_peek(lexer);

    if (token_is_invalid(t)) {
        log_parserr(t, "Invalid token: %s\n", t.text);
        return NULL;
    }

    if (t.type == T_NU) {
        char *num_end;
    	long number = strtol(t.text, &num_end, 10);

        if (*num_end != '\0') {
            log_parserr(t, "Invalid number: %s", t.text);
            return NULL;
        }

        AST *ast = malloc(sizeof(AST));
        ast->type = AST_NUMBER;
        ast->number = number;

        return ast;
    }

    if (t.type == T_ID) {
        AST *ast = malloc(sizeof(AST));
        ast->type = AST_SYMBOL;

        strncpy(ast->symbol, t.text, sizeof(ast->symbol));

        return ast;
    }

    if (t.type == T_IF) {
        AST *ast = malloc(sizeof(AST));
        ast->type = AST_IF;

        AST *condition = parse_expression(lexer);
        token t_then = lexer_peek(lexer);

        if (t_then.type != T_TN) {
            log_parserr(t_then, "Expected 'then' after if condition\n");
            return NULL;
        }

        AST *consequence = parse_expression(lexer);
        token t_else = lexer_peek(lexer);

        if (t_else.type != T_EL) {
            log_parserr(t_else, "Expected 'else' after if consequence\n");
            return NULL;
        }

        AST *alternative = parse_expression(lexer);

        ast->if_statement.condition = condition;
        ast->if_statement.consequence = consequence;
        ast->if_statement.alternative = alternative;

        return ast;
    }

/*
    if (token.type == T_LP) {
        AST *ast = malloc(sizeof(AST));
        ast->type = AST_LIST;
        ast->list = malloc(sizeof(AST_LIST));
        ast->list->length = 0;
        ast->list->items = malloc(sizeof(AST *) * 10);

        while (1) {
            AST *item = parse_expression(lexer);

            if (item == NULL) {
                break;
            }

            ast->list->items[ast->list->length] = item;
            ast->list->length += 1;
        }

        return ast;
    }
    */

    log_parserr(t, "Invalid token");

    return NULL;
}

AST *parse(lexer *lexer) {
    return parse_expression(lexer);
}

AST *parse_file(const char *filename) {
    lexer lexer = lexer_from_file(filename);
    AST *ast = parse(&lexer);

    return ast;
}

void print_ast(const AST* ast) {
    if (ast == NULL) {
        return;
    }

    if (ast->type == AST_NUMBER) {
        printf("NUMBER(%d)", ast->number);
    }

    if (ast->type == AST_SYMBOL) {
        printf("SYMBOL(%s)", ast->symbol);
    }

    if (ast->type == AST_IF) {
        printf("IF(");
        print_ast(ast->if_statement.condition);
        printf(" ");
        print_ast(ast->if_statement.consequence);
        printf(" ");
        print_ast(ast->if_statement.alternative);
        printf(")");
    }
/*
    if (ast->type == AST_LIST) {
        printf("(");

        for (int i = 0; i < ast->list->length; i++) {
            print_ast(ast->list->items[i]);

            if (i < ast->list->length - 1) {
                printf(" ");
            }
        }

        printf(")");
    }*/
}