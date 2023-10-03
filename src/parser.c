#include "parser.h"
#include "aux.h"
#include <stdlib.h>
#include <string.h>

#define log_parserr(tk, ...) {fprintf(stderr, "error at %d:%d \"%s\": ", (tk).line, (tk).column, (tk).text); fprintf(stderr, __VA_ARGS__);}

AST *parse_expression(Lexer *lexer);
AST *parse_comparison(Lexer *lexer);
AST *parse_term(Lexer *lexer);
AST *parse_factor(Lexer *lexer);
AST *parse_atom(Lexer *lexer);
AST *parse_atom_number(Token token);
AST *parse_atom_symbol(Token token);
AST *parse_atom_if(Lexer *lexer, Token first_token);
AST *parse_statement(Lexer *);
AST *parse_assignment(Lexer *);
AST *parse_sequence(Lexer *);
AST *parse_expression_statement(Lexer *);

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

int consume_expected_token(Lexer *lexer, TokenType type, Token *token, const char *message) {
  Token t = lexer_pop(lexer);
  if (!teq(t, type)) {
    log_parserr(t, "expected: %s, found: %d\n", message, type);
    return 0;
  }
  if (token) {
    *token = t;
  }
  return 1;
}

AST *parse(Lexer *lexer) {
    if (!lexer) { return NULL; }
    return parse_sequence(lexer);
}

AST *parse_file(FILE *file) {
  if (!file) { return NULL; }

  Lexer *lexer = lexer_from_file(file);
  AST *ast = parse(lexer);

  lexer_destroy(&lexer);

  return ast;
}

AST *parse_statement(Lexer *lexer) {
  Token t = lexer_peek(lexer);

  switch (t.type) {
  case T_VL:
    return parse_assignment(lexer);
  default:
    return parse_expression_statement(lexer);
  }
}

AST *parse_sequence(Lexer *lexer) {
  AST *ast = ast_new();
  ast->type = AST_STMT_SEQ;
  ast->sequence = NULL;

  AST *statement = NULL;

  while (tvalid(lexer_peek(lexer))) {
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

AST *parse_expression_statement(Lexer *lexer) {
  AST *expression = parse_expression(lexer);
  if (!expression) { return NULL; }
  AST *ast = ast_new();
  ast->type = AST_STMT_EXP;
  ast->first_token = expression->first_token;
  ast->last_token = expression->last_token;
  ast->expression = expression;
  return ast;
}

AST *parse_assignment(Lexer *lexer) {
  Token t_val = {0};
  Token t_id = {0};

  if (!consume_expected_token(lexer, T_VL, &t_val, "val")) { return NULL; }
  if (!consume_expected_token(lexer, T_ID, &t_id, "identifier")) { return NULL; }
  if (!consume_expected_token(lexer, T_EQ, NULL, "\"=\"")) { return NULL; }

  AST *expression = parse_expression(lexer);

  if (!expression) {
    log_parserr(t_id, "failed to parse assignment expression");
    return NULL;
  }

  AST *ast = ast_new();
  ast->type = AST_STMT_ASN;
  ast->first_token = t_val;
  ast->last_token = expression->last_token;
  strncpy(ast->assignment.identifier, t_id.text, sizeof(ast->assignment.identifier));
  ast->assignment.expression = expression;

  return ast;
}

AST *parse_expression(Lexer *lexer) {
  if (!lexer) { return NULL; }
  return parse_comparison(lexer);
}

Operator comparison_operator(Token t) {
  switch (t.type) {
  case T_GT: return O_GT;
  case T_LT: return O_LT;
  default:
    return 0;
  }
}

AST *parse_comparison(Lexer *lexer) {
  if (!lexer) { return NULL; }

  AST *term = parse_term(lexer);
  Operator op = 0;

  while (tvalid(lexer_peek(lexer)) && 
    (op = comparison_operator(lexer_peek(lexer)))) 
  {
    lexer_pop(lexer);
    AST *term2 = parse_term(lexer);
    AST *ast = ast_new();
    ast->type = AST_BINARY;
    ast->binary_expression.operator = op;
    ast->binary_expression.left = term;
    ast->binary_expression.right = term2;
    ast->first_token = term->first_token;
    ast->last_token = term2->last_token;
    term = ast;
  }
  
  return term;
}

Operator term_operator(Token t) {
  switch (t.type) {
  case T_PL: return O_PL;
  case T_MI: return O_MI;
  default:
    return 0;
  }
}

AST *parse_term(Lexer *lexer) {
  AST *factor = parse_factor(lexer);
  Operator op = 0;

  while (tvalid(lexer_peek(lexer)) && 
    (op = term_operator(lexer_peek(lexer)))) 
  {
    lexer_pop(lexer);
    AST *factor2 = parse_factor(lexer);
    AST *ast = ast_new();
    ast->type = AST_BINARY;
    ast->binary_expression.operator = op;
    ast->binary_expression.left = factor;
    ast->binary_expression.right = factor2;
    ast->first_token = factor->first_token;
    ast->last_token = factor2->last_token;
    factor = ast;
  }
  
  return factor;
}

Operator factor_operator(Token t) {
  switch (t.type) {
  case T_MU: return O_MU;
  case T_DI: return O_DI;
  default:
    return 0;
  }
}

AST *parse_factor(Lexer *lexer) {
  AST *atom = parse_atom(lexer);
  Operator op = 0;
  while (tvalid(lexer_peek(lexer)) &&
    (op = factor_operator(lexer_peek(lexer)))) 
  {
    lexer_pop(lexer);
    AST *atom2 = parse_atom(lexer);
    AST *ast = ast_new();
    ast->type = AST_BINARY;
    ast->binary_expression.operator = op;
    ast->binary_expression.left = atom;
    ast->binary_expression.right = atom2;
    ast->first_token = atom->first_token;
    ast->last_token = atom2->last_token;
    atom = ast;
  }
  return atom;
}

AST *parse_atom(Lexer *lexer) {
  if (!lexer) { return NULL; }

  Token t = lexer_pop(lexer);
  AST *ast = NULL;
  Token first_token = {0};
  
  switch (t.type) {
  case T_NU:
    ast = parse_atom_number(t);
    return ast;
  case T_ID:
    ast = parse_atom_symbol(t);
    return ast;
  case T_IF:
    first_token = t;
    ast = parse_atom_if(lexer, first_token);
    return ast;
  case T_LP:
    ast = parse_expression(lexer);
    if (!consume_expected_token(lexer, T_RP, NULL, "')'")) {
      ast_destroy(&ast);
      return NULL;
    }
    return ast;
  case T_IV:
    log_parserr(t, "Invalid token: \"%s\"\n", t.text);
    return NULL;
  default:
    log_parserr(t, "Unexpected token of type %d: \"%s\"\n", t.type, t.text);
    return NULL;
  }
}

AST *parse_atom_if_parts(
  Lexer *lexer, 
  Token first_token,
  AST **condition,
  AST **consequence,
  AST **alternative
) {
  if (!(*condition = parse_expression(lexer))) { return NULL; }
  if (!consume_expected_token(lexer, T_TN, NULL, "'then'")) { return NULL; }
  if (!(*consequence = parse_expression(lexer))) { return NULL; }
  if (!consume_expected_token(lexer, T_EL, NULL, "'else'")) { return NULL; }
  if (!(*alternative = parse_expression(lexer))) { return NULL; }

  Token last_token = (*consequence)->last_token;
  AST *ast = ast_new();

  ast->type = AST_IF;
  ast->first_token = first_token;
  ast->last_token = last_token;
  ast->if_statement.condition = *condition;
  ast->if_statement.consequence = *consequence;
  ast->if_statement.alternative = *alternative;

  return ast;
}

AST *parse_atom_if(Lexer *lexer, Token first_token) {
  AST *condition = NULL;
  AST *consequence = NULL;
  AST *alternative = NULL;
  AST *ast = parse_atom_if_parts(lexer, first_token, &condition, &consequence, &alternative);

  if (!ast) {
    ast_destroy(&condition);
    ast_destroy(&consequence);
    ast_destroy(&alternative);
  }

  return ast;
}

AST *parse_atom_symbol(Token t) {
  AST *ast = ast_new();

  ast->type = AST_SYMBOL;
  strncpy(ast->symbol, t.text, sizeof(ast->symbol));
  ast->first_token = t;
  ast->last_token = t;

  return ast;
}

AST *parse_atom_number(Token t) {
  long number = 0;

  if (!ascii_to_long(t.text, &number)) {
      log_parserr(t, "Invalid number: %s\n", t.text);
      return NULL;
  }

  AST *ast = ast_new();
  ast->type = AST_NUMBER;
  ast->number = number;
  ast->first_token = t;
  ast->last_token = t;

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
