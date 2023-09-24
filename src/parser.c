#include "parser.h"
#include "lexer.h"
#include "aux.h"
#include <stdlib.h>
#include <string.h>

#define log_parserr(tk, ...) {fprintf(stderr, "error at %d:%d \"%s\": ", (tk) ? (tk)->line : -1, (tk) ? (tk)->column : -1, (tk) ? (tk)->text : "NULL");fprintf(stderr, __VA_ARGS__);}

//#define log_debug(...) {printf("[%d] ", __LINE__); printf(__VA_ARGS__);}
int log_debug(const char *p, ...) { (void)p; return 0; }

AST *parse_expression(lexer *lexer);
AST *parse_expression_p(lexer *lexer, Operator *operator);
AST *parse_comparison(lexer *lexer);
AST *parse_comparison_p(lexer *lexer, Operator *operator);
AST *parse_term(lexer *lexer);
AST *parse_term_p(lexer *lexer, Operator *operator);
AST *parse_factor(lexer *lexer);
AST *parse_factor_p(lexer *lexer, Operator *operator);
AST *parse_atom(lexer *lexer);

AST *ast_new() {
  AST *ast = malloc(sizeof(AST));
  memset(ast, 0, sizeof(AST));
  return ast;
}

int ast_destroy(AST **ast) {
  if (!(ast && *ast)) {
    return 0;
  }

  switch ((*ast)->type) {
  case AST_NUMBER:
  case AST_SYMBOL:
    break;
  case AST_IF:
    if (!ast_destroy(&(*ast)->if_statement.condition)) { return 0; }
    if (!ast_destroy(&(*ast)->if_statement.consequence)) { return 0; }
    if (!ast_destroy(&(*ast)->if_statement.alternative)) { return 0; }
    break;
  case AST_BINARY:
    if (!ast_destroy(&(*ast)->binary_expression.left)) { return 0; }
    if (!ast_destroy(&(*ast)->binary_expression.right)) { return 0; }
    break;
  default:
    fprintf(stderr, "internal error during ast destruction: unhandled ast type %d\n", (*ast)->type);
    exit(91);
  }

  free(*ast);
  *ast = NULL;

  return 1;
}

AST *parse(lexer *lexer) {
  if (!lexer) { return NULL; }
  return parse_expression(lexer);
}

  AST *parse_file(FILE *file) {
  if (!file) { return NULL; }
  return parse(lexer_from_file(file));
}

AST *combine(Operator operator, AST *left, AST *right) {
  if (!right) return left;

  AST *b = ast_new();

  b->type = AST_BINARY;
  b->binary_expression.operator = operator;
  b->binary_expression.left = left;
  b->binary_expression.right = right;
  
  return b;
}

AST *fix_associativity(AST *ast) {
  if (!ast) { return NULL; }

  switch (ast->type) {
  case AST_NUMBER:
  case AST_SYMBOL:
    return ast;
  case AST_IF:
    ast->if_statement.condition = fix_associativity(ast->if_statement.condition);
    ast->if_statement.consequence = fix_associativity(ast->if_statement.consequence);
    ast->if_statement.alternative = fix_associativity(ast->if_statement.alternative);
    return ast;
	case AST_BINARY:
    ast->binary_expression.right = fix_associativity(ast->binary_expression.right);

    Operator operator = ast->binary_expression.operator;

    if ((operator == O_MI || operator == O_DI) && 
        ast->binary_expression.right->type == AST_BINARY &&
        ast->binary_expression.right->binary_expression.operator == operator
    ) {
      AST *right = ast->binary_expression.right;

      ast->binary_expression.right = right->binary_expression.right;
      right->binary_expression.right = right->binary_expression.left;
      right->binary_expression.left = ast->binary_expression.left;
      ast->binary_expression.left = right;
    }

    ast->binary_expression.left = fix_associativity(ast->binary_expression.left);

    return ast;
  default:
    fprintf(stderr, "internal error during associativity fix: unhandled ast type %d\n", ast->type);
    exit(91);
  }
}

AST *parse_expression(lexer *lexer) {
  if (!lexer) { return NULL; }
  
  AST *ast = parse_comparison(lexer);

  if (ast) {
    ast = fix_associativity(ast);
  }

  return ast;
}

AST *parse_comparison(lexer *lexer) {
  if (!lexer) { return NULL; }

  Operator operator = 0;
  AST *term = parse_term(lexer);
  AST *comparison_p = parse_comparison_p(lexer, &operator);
  
  return combine(operator, term, comparison_p);
}

AST *parse_comparison_p(lexer *lexer, Operator *operator) {
  if (!lexer) { return NULL; }

  token *t = lexer_peek(lexer);
  switch (t->type) {
  case T_GT: *operator = O_GT; break;
  case T_LT: *operator = O_LT; break;
  default:
    return NULL;
  }

  lexer_pop(lexer);
  Operator o2 = 0;
  AST *term = parse_term(lexer);
  AST *comparison_p = parse_comparison_p(lexer, &o2);
  return combine(o2, term, comparison_p);
}

AST *parse_term(lexer *lexer) {
  Operator operator = 0;
  AST *factor = parse_factor(lexer);
  AST *term_p = parse_term_p(lexer, &operator);
  return combine(operator, factor, term_p);
}

/*
AST *recurse(lexer *lex, AST* (*parse_subexpression)(lexer*), Operator *operator) {
  *operator = 0;
  AST *left = parse_subexpression(lex);
  AST *right = recurse(lex, parse_subexpression, &operator);
  return combine(operator, left, right);
}
*/

AST *parse_term_p(lexer *lexer, Operator *operator) {
  token *t = lexer_peek(lexer); 
  switch (t->type) {
  case T_PL: *operator = O_PL; break;
  case T_MI: *operator = O_MI; break;
  default: return NULL;
  }

  lexer_pop(lexer);
  Operator o2 = 0;
  AST *factor = parse_factor(lexer);
  AST *term_p = parse_term_p(lexer, &o2);

  return combine(o2, factor, term_p);
}

AST *parse_factor(lexer *lexer) {
  Operator operator = 0;
  AST *atom = parse_atom(lexer);
  AST *factor_p = parse_factor_p(lexer, &operator);
  return combine(operator, atom, factor_p);
}

AST *parse_factor_p(lexer *lexer, Operator *operator) {
  token *t = lexer_peek(lexer); 
  switch (t->type) {
  case T_MU: *operator = O_MU; break;
  case T_DI: *operator = O_DI; break;
  default: return NULL;
  }

  lexer_pop(lexer);
  Operator o2 = 0;
  AST *atom = parse_atom(lexer);
  AST *factor_p = parse_factor_p(lexer, &o2);
  return combine(o2, atom, factor_p);
}

AST *parse_atom(lexer *lexer) {
  if (!lexer) { return NULL; }

  token *t = lexer_peek(lexer);

  if (token_is_invalid(t)) {
      log_parserr(t, "Invalid token: %s\n", t->text);
      return NULL;
  }

  long number = 0;
  AST *ast = NULL;
  AST *condition = NULL;
  AST *consequence = NULL;
  AST *alternative = NULL;

  switch (t->type) {
  case T_NU:
    if (!ascii_to_long(t->text, &number)) {
        log_parserr(t, "Invalid number: %s\n", t->text);
        return NULL;
    }

    ast = ast_new();
    ast->type = AST_NUMBER;
    ast->number = number;

    lexer_pop(lexer);
    log_debug("parsed number\n");

    return ast;
  case T_ID:
    ast = ast_new();
    ast->type = AST_SYMBOL;

    strncpy(ast->symbol, t->text, sizeof(ast->symbol));

    lexer_pop(lexer);
    log_debug("parsed identifier\n");

    return ast;
  case T_IF:
    log_debug("beginning if\n");
    lexer_pop(lexer);

    ast = ast_new();
    ast->type = AST_IF;

    condition = parse_expression(lexer);
    log_debug("parsed condition\n");
    token *t_then = lexer_pop(lexer);

    if (t_then == NULL || t_then->type != T_TN) {
      log_parserr(t_then, "Expected 'then' after if condition\n");
      return NULL;
    }

    consequence = parse_expression(lexer);
    log_debug("parsed consequence\n");
    token *t_else = lexer_pop(lexer);

    if (t_else == NULL || t_else->type != T_EL) {
        log_parserr(t_else, "Expected 'else' after if consequence\n");
        return NULL;
    }

    alternative = parse_expression(lexer);
    log_debug("parsed alternative\n");

    ast->if_statement.condition = condition;
    ast->if_statement.consequence = consequence;
    ast->if_statement.alternative = alternative;

    return ast;
  default:
    log_parserr(t, "Unexpected token of type %d: \"%s\"\n", t->type, t->text);
    return NULL;
  }
}

void print_ast(const AST* ast) {
  if (ast == NULL) {
    return;
  }

  if (ast->type == AST_NUMBER) {
    printf("NUMBER(%ld)", ast->number);
  }

  if (ast->type == AST_SYMBOL) {
    printf("SYMBOL(%s)", ast->symbol);
  }

  if (ast->type == AST_BINARY) {
    printf("(");
    print_ast(ast->binary_expression.left);
    switch (ast->binary_expression.operator) {
    case O_GT: printf(" > "); break;
    case O_LT: printf(" < "); break;
    case O_PL: printf(" + "); break;
    case O_MI: printf(" - "); break;
    case O_MU: printf(" * "); break;
    case O_DI: printf(" / "); break;
    default: printf(" ? "); break;
    }
    print_ast(ast->binary_expression.right);
    printf(")");
  }

  if (ast->type == AST_IF) {
    printf("[IF ");
    print_ast(ast->if_statement.condition);
    printf(" THEN ");
    print_ast(ast->if_statement.consequence);
    printf(" ELSE ");
    print_ast(ast->if_statement.alternative);
    printf("]");
  }
}