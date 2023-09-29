#include "types.h"

#define PRINT_AST_LOCATION(ast) fprintf(stderr, "type error in line %d, column %d: ", (ast)->first_token ? (ast)->first_token->line + 1 : -1, (ast)->first_token ? (ast)->first_token->column + 1 : -1);

static const Type TYPE_UNDEFINED = {0};
static const Type TYPE_NUMBER = {TY_NUMBER};
static const Type TYPE_BOOLEAN = {TY_BOOLEAN};

Type typecheck_binary_expression(AST*);
Type typecheck_if_expression(AST*);
Type lower_bound(Type s, Type t);
int is_number(Type);
int is_boolean(Type);

Type typecheck(AST *ast) {
  if (!ast) { return TYPE_UNDEFINED; }
  switch (ast->type) {
  case AST_NUMBER:
    return TYPE_NUMBER;
  case AST_BINARY:
    return typecheck_binary_expression(ast);
  case AST_IF:
    return typecheck_if_expression(ast);
  }
}

Type typecheck_if_expression(AST *ast) {
  Type t_condition = typecheck(ast->if_statement.condition);
  Type t_consequence = typecheck(ast->if_statement.consequence);
  Type t_alternative = typecheck(ast->if_statement.alternative);

  if (is_boolean(t_condition)) {
    return lower_bound(t_consequence, t_alternative);
  } else {
    PRINT_AST_LOCATION(ast->if_statement.condition);
    fprintf(stderr, "\"if\" condition must be of type boolean\n");
    return TYPE_UNDEFINED;
  }
}

Type typecheck_binary_expression(AST *ast) {
  Type t_left = typecheck(ast->binary_expression.left);
  Type t_right = typecheck(ast->binary_expression.right);

  switch (ast->binary_expression.operator) {
  case O_GT:
  case O_LT:
  case O_EQ:
  case O_NE:
    if (is_number(t_left) && is_number(t_right)) {
      return TYPE_BOOLEAN;
    } else {
      return TYPE_UNDEFINED;
    }
  case O_PL:
  case O_MI:
  case O_MU:
  case O_DI:
    if (is_number(t_left) && is_number(t_right)) {
      return TYPE_NUMBER;
    } else {
      return TYPE_UNDEFINED;
    }
  case O_UNDEFINED:
    fprintf(stderr, "internal error: undefined operator\n");
    return TYPE_UNDEFINED;
  }
}

Type lower_bound(Type s, Type t) {
  if (s.type == TY_BOOLEAN && t.type == TY_BOOLEAN) { return TYPE_BOOLEAN; }
  if (s.type == TY_NUMBER && t.type == TY_NUMBER) { return TYPE_NUMBER; }
  return TYPE_UNDEFINED;
}

void print_type(Type t) {
  switch (t.type) {
  case TY_NUMBER:
    printf("NUMBER");
    break;
  case TY_BOOLEAN:
    printf("BOOLEAN");
    break;
  case TY_UNDEFINED:
    printf("UNDEFINED");
    break;
  default:
    printf("???");
    break;
  }
}

int is_number(Type t) {
  return t.type == TY_NUMBER;
}

int is_boolean(Type t) {
  return t.type == TY_BOOLEAN;
}