#include "parser.h"
#include "aux.h"
#include <stdlib.h>
#include <string.h>

#define log_parserr(tk, ...) {fprintf(stderr, "error at %d:%d \"%s\": ", (tk) ? (tk)->line : -1, (tk) ? (tk)->column : -1, (tk) ? (tk)->text : "NULL");fprintf(stderr, __VA_ARGS__);}

//#define log_debug(...) {printf("[%d] ", __LINE__); printf(__VA_ARGS__);}
//int log_debug(const char *p, ...) { (void)p; return 0; }

AST *parse_expression(lexer *lexer);
AST *parse_comparison(lexer *lexer);
AST *parse_term(lexer *lexer);
AST *parse_factor(lexer *lexer);
AST *parse_atom(lexer *lexer);
AST *parse_atom_number(token *token);
AST *parse_atom_symbol(token *token);
AST *parse_atom_if(lexer *lexer, token *first_token);
AST *parse_statement(lexer*);
AST *parse_assignment(lexer*);
AST *parse_sequence(lexer*);
AST *parse_expression_statement(lexer*);

AST *ast_new(void) {
  AST *ast = malloc(sizeof(AST));
  memset(ast, 0, sizeof(AST));
  return ast;
}

int ast_destroy_sequence(AST_SEQUENCE **seq) {
  if (!(seq && *seq)) { return 0; }

  if ((*seq)->next) {
    ast_destroy_sequence(&(*seq)->next);
  }

  if ((*seq)->statement) {
    ast_destroy(&(*seq)->statement);
  }

  free(*seq);
  *seq = NULL;

  return 1; 
}

int ast_destroy(AST **ast) {
  if (!(ast && *ast)) {
    return 0;
  }

  if ((*ast)->first_token) {
    token_destroy((*ast)->first_token);
    (*ast)->first_token = NULL;
  }
  if ((*ast)->last_token) {
    token_destroy((*ast)->last_token);
    (*ast)->last_token = NULL;
  }
  switch ((*ast)->type) {
  case AST_NUMBER:
  case AST_SYMBOL:
  case AST_UNDEFINED:
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
  case AST_STMT_EXP:
    if (!ast_destroy(&(*ast)->expression)) { return 0; }
    break;
  case AST_STMT_ASN:
    if (!ast_destroy(&(*ast)->assignment.expression)) { return 0; }
    break;
  case AST_STMT_SEQ:
    if (!ast_destroy_sequence(&(*ast)->sequence)) { return 0; }
    break;
  }

  free(*ast);
  *ast = NULL;

  return 1;
}

AST *parse(lexer *lexer) {
    if (!lexer) { return NULL; }
    
    return parse_sequence(lexer);
}

AST *parse_file(FILE *file) {
  if (!file) { return NULL; }

  lexer *lexer = lexer_from_file(file);
  AST *ast = parse(lexer);

  lexer_destroy(&lexer);

  return ast;
}

AST *parse_statement(lexer *lexer) {
  token *t = lexer_peek(lexer);

  if (!t) { return NULL; }

  switch (t->type) {
  case T_VL:
    return parse_assignment(lexer);
  default:
    return parse_expression_statement(lexer);
  }
}

AST *parse_sequence(lexer *lexer) {
  AST *ast = ast_new();
  ast->type = AST_STMT_SEQ;
  ast->sequence = NULL;

  AST *statement = NULL;

  while (lexer_peek(lexer)) {
    statement = parse_statement(lexer);
    if (!ast->sequence) {
      ast->sequence = malloc(sizeof(AST_SEQUENCE));
      ast->sequence->statement = statement;
      ast->sequence->next = NULL;
    } else {
      AST_SEQUENCE *seq = ast->sequence;
      while (seq->next) {
        seq = seq->next;
      }
      seq->next = malloc(sizeof(AST_SEQUENCE));
      seq->next->statement = statement;
      seq->next->next = NULL;
    }
  }

  return ast;
}

AST *parse_expression_statement(lexer *lexer) {
  AST *expression = parse_expression(lexer);

  if (!expression) { return NULL; }

  AST *ast = ast_new();

  ast->type = AST_STMT_EXP;
  ast->first_token = token_copy(expression->first_token);
  ast->last_token = token_copy(expression->last_token);
  ast->expression = expression;
  return ast;
}

AST *parse_assignment(lexer *lexer) {
  token *t_val = lexer_pop(lexer);
  if (!(t_val && t_val->type == T_VL)) {
    log_parserr(t_val, "internal error: expected \"val\"");
    token_destroy(t_val);
    return NULL;
  }

  token *t_id = lexer_pop(lexer);
  
  if (!(t_id && t_id->type == T_ID)) {
    log_parserr(t_id, "expected: identifier");
    token_destroy(t_val);
    token_destroy(t_id);
    return NULL;
  }

  token *t_eq = lexer_pop(lexer);

  if (!(t_eq && t_eq->type == T_EQ)) {
    log_parserr(t_eq, "expected: \"=\"");
    token_destroy(t_val);
    token_destroy(t_id);
    token_destroy(t_eq);
  }

  AST *expression = parse_expression(lexer);

  if (!expression) {
    log_parserr(t_eq, "failed to parse assignment expression");
    token_destroy(t_val);
    token_destroy(t_id);
    token_destroy(t_eq);
    return NULL;
  }

  AST *ast = ast_new();
  ast->type = AST_STMT_ASN;
  ast->first_token = t_val;
  ast->last_token = token_copy(expression->last_token);
  strncpy(ast->assignment.identifier, t_id->text, sizeof(ast->assignment.identifier));
  ast->assignment.expression = expression;
  
  token_destroy(t_id);
  token_destroy(t_eq);

  return ast;
}

AST *parse_expression(lexer *lexer) {
  if (!lexer) { return NULL; }
  
  AST *ast = parse_comparison(lexer);

  return ast;
}

Operator comparison_operator(token *t) {
  if (!t) { return 0; }

  switch (t->type) {
  case T_GT: return O_GT;
  case T_LT: return O_LT;
  default:
    return 0;
  }
}

AST *parse_comparison(lexer *lexer) {
  if (!lexer) { return NULL; }

  AST *term = parse_term(lexer);
  Operator op = 0;

  while (lexer_peek(lexer) && (op = comparison_operator(lexer_peek(lexer)))) {
    token_destroy(lexer_pop(lexer));
    AST *term2 = parse_term(lexer);
    AST *ast = ast_new();
    ast->type = AST_BINARY;
    ast->binary_expression.operator = op;
    ast->binary_expression.left = term;
    ast->binary_expression.right = term2;
    ast->first_token = token_copy(term->first_token);
    ast->last_token = token_copy(term2->last_token);
    term = ast;
  }
  
  return term;
}

Operator term_operator(token *t) {
  if (!t) { return 0; }

  switch (t->type) {
  case T_PL: return O_PL;
  case T_MI: return O_MI;
  default:
    return 0;
  }
}

AST *parse_term(lexer *lexer) {
  AST *factor = parse_factor(lexer);
  Operator op = 0;

  while (lexer_peek(lexer) && (op = term_operator(lexer_peek(lexer)))) {
    token_destroy(lexer_pop(lexer));
    AST *factor2 = parse_factor(lexer);
    AST *ast = ast_new();
    ast->type = AST_BINARY;
    ast->binary_expression.operator = op;
    ast->binary_expression.left = factor;
    ast->binary_expression.right = factor2;
    ast->first_token = token_copy(factor->first_token);
    ast->last_token = token_copy(factor2->last_token);
    factor = ast;
  }
  
  return factor;
}

Operator factor_operator(token *t) {
  if (!t) { return 0; }

  switch (t->type) {
  case T_MU: return O_MU;
  case T_DI: return O_DI;
  default:
    return 0;
  }
}

AST *parse_factor(lexer *lexer) {
  AST *atom = parse_atom(lexer);
  Operator op = 0;
  while (lexer_peek(lexer) && (op = factor_operator(lexer_peek(lexer)))) {
    token_destroy(lexer_pop(lexer));
    AST *atom2 = parse_atom(lexer);
    AST *ast = ast_new();
    ast->type = AST_BINARY;
    ast->binary_expression.operator = op;
    ast->binary_expression.left = atom;
    ast->binary_expression.right = atom2;
    ast->first_token = token_copy(atom->first_token);
    ast->last_token = token_copy(atom2->last_token);
    atom = ast;
  }
  return atom;
}

AST *parse_atom(lexer *lexer) {
  if (!lexer) { return NULL; }

  token *t = lexer_peek(lexer);

  if (token_is_invalid(t)) {
      log_parserr(t, "Invalid token: %s\n", t->text);
      return NULL;
  }

  AST *ast = NULL;
  token *first_token = NULL;
  
  switch (t->type) {
  case T_NU:
    ast = parse_atom_number(t);
    token_destroy(lexer_pop(lexer));
    return ast;
  case T_ID:
    ast = parse_atom_symbol(t);
    token_destroy(lexer_pop(lexer));
    return ast;
  case T_IF:
    first_token = token_copy(t);
    token_destroy(lexer_pop(lexer));
    ast = parse_atom_if(lexer, first_token);
    return ast;
  case T_LP:
    token_destroy(lexer_pop(lexer));
    ast = parse_expression(lexer);
    t = lexer_pop(lexer);
    if (!(t && t->type == T_RP)) {
      log_parserr(t, "Expected ')'\n");
      token_destroy(t);
      ast_destroy(&ast);
      return NULL;
    }
    token_destroy(t);
    return ast;
  default:
    log_parserr(t, "Unexpected token of type %d: \"%s\"\n", t->type, t->text);
    return NULL;
  }
}

AST *parse_atom_if(lexer *lexer, token *first_token) {
  AST *condition = parse_expression(lexer);
  
  if (!condition) {
    return NULL;
  }

  token *t_then = lexer_pop(lexer);

  if (t_then == NULL || t_then->type != T_TN) {
    log_parserr(t_then, "Expected 'then' after if condition\n");
    ast_destroy(&condition);
    return NULL;
  }

  token_destroy(t_then);

  AST *consequence = parse_expression(lexer);

  if (!consequence) {
    ast_destroy(&condition);
    return NULL;
  }

  token *t_else = lexer_pop(lexer);

  if (t_else == NULL || t_else->type != T_EL) {
      log_parserr(t_else, "Expected 'else' after if consequence\n");
      token_destroy(t_else);
      ast_destroy(&condition);
      ast_destroy(&consequence);
      return NULL;
  }

  token_destroy(t_else);

  AST *alternative = parse_expression(lexer);

  if (!alternative) {
    ast_destroy(&condition);
    ast_destroy(&consequence);
    return NULL;
  }

  token *last_token = token_copy(consequence->last_token);
  AST *ast = ast_new();

  ast->type = AST_IF;
  ast->first_token = first_token;
  ast->last_token = last_token;
  ast->if_statement.condition = condition;
  ast->if_statement.consequence = consequence;
  ast->if_statement.alternative = alternative;

  return ast;
}

AST *parse_atom_symbol(token *t) {
  AST *ast = ast_new();

  ast->type = AST_SYMBOL;
  strncpy(ast->symbol, t->text, sizeof(ast->symbol));
  ast->first_token = token_copy(t);
  ast->last_token = token_copy(t);

  return ast;
}

AST *parse_atom_number(token *t) {
  long number = 0;

  if (!ascii_to_long(t->text, &number)) {
      log_parserr(t, "Invalid number: %s\n", t->text);
      return NULL;
  }

  AST *ast = ast_new();
  ast->type = AST_NUMBER;
  ast->number = number;
  ast->first_token = token_copy(t);
  ast->last_token = token_copy(t);

  return ast;
}

void print_ast(const AST* ast) {
  if (ast == NULL) {
    return;
  }

  // int line_start = ast->first_token ? ast->first_token->line : 0;
  // int col_start = ast->first_token ? ast->first_token->column : 0;
  // int line_end = ast->last_token ? ast->first_token->line : 0;
  // int col_end = ast->last_token ? ast->first_token->column : 0;
  //printf("@%d,%d..%d,%d ", line_start, col_start, line_end, col_end);

  switch (ast->type) {
  case AST_NUMBER:
    printf("NUMBER(%ld)", ast->number);
    break;
  case AST_SYMBOL:
    printf("SYMBOL(%s)", ast->symbol);
    break; 
  case AST_BINARY:
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
    break;
  case AST_IF:
    printf("[IF ");
    print_ast(ast->if_statement.condition);
    printf(" THEN ");
    print_ast(ast->if_statement.consequence);
    printf(" ELSE ");
    print_ast(ast->if_statement.alternative);
    printf("]");
    break;
  case AST_STMT_EXP:
    printf("<");
    print_ast(ast->expression);
    printf(">");
    break;
  case AST_STMT_SEQ:
    printf("<");
    AST_SEQUENCE *seq = ast->sequence;
    while (seq) {
      print_ast(seq->statement);
      seq = seq->next;
      if (seq) {
        printf("; ");
      }
    }
    printf(">");
    break;
  case AST_STMT_ASN:
    printf("<%s = ", ast->assignment.identifier);
    print_ast(ast->assignment.expression);
    printf(">");
    break;
  case AST_UNDEFINED:
    printf("<UNDEFINED>");
    break;
  }
}
