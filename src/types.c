#include "types.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//#define SET_ERROR(ast, message) fprintf(stderr, "type error in line %d, column %d: %s\n", (ast)->first_token ? (ast)->first_token->line + 1 : -1, (ast)->first_token ? (ast)->first_token->column + 1 : -1, message);
#define SET_ERROR(ast, message) snprintf(ERROR_BUFFER, sizeof(ERROR_BUFFER), "type error in line %d, column %d: %s\n", (ast)->first_token ? (ast)->first_token->line + 1 : -1, (ast)->first_token ? (ast)->first_token->column + 1 : -1, message);
#define CLEAR_ERROR() ERROR_BUFFER[0] = '\0';

static const Type TYPE_UNDEFINED = {0};
static const Type TYPE_NUMBER = {TY_NUMBER};
static const Type TYPE_BOOLEAN = {TY_BOOLEAN};
static const Type TYPE_UNIT = {TY_UNIT};

static char ERROR_BUFFER[4096] = {0};

Type typecheck_binary_expression(Tenv*, AST*);
Type typecheck_if_expression(Tenv*, AST*);
Type typecheck_expression_statement(Tenv*, AST*);
Type typecheck_assignment(Tenv*, AST*);
Type typecheck_sequence(Tenv*, AST*);
Type lower_bound(Type s, Type t);
int is_number(Type);
int is_boolean(Type);

const char *typecheck_last_error() {
  return ERROR_BUFFER;
}

Type typecheck(Tenv *tenv, AST *ast) {
  CLEAR_ERROR();

  if (!ast) { return TYPE_UNDEFINED; }
  switch (ast->type) {
  case AST_NUMBER:
    return TYPE_NUMBER;
  case AST_BINARY:
    return typecheck_binary_expression(tenv, ast);
  case AST_IF:
    return typecheck_if_expression(tenv, ast);
  case AST_STMT_EXP:
    return typecheck_expression_statement(tenv, ast);
  case AST_STMT_ASN:
    return typecheck_assignment(tenv, ast);
  case AST_STMT_SEQ:
    return typecheck_sequence(tenv, ast);
  case AST_SYMBOL:
    return tenv_lookup(tenv, ast->symbol);
  case AST_UNDEFINED:
    SET_ERROR(ast, "internal error: ast type undefined");
    return TYPE_UNDEFINED;
  }
}

Type typecheck_if_expression(Tenv *tenv, AST *ast) {
  Type t_condition = typecheck(tenv, ast->if_statement.condition);
  Type t_consequence = typecheck(tenv, ast->if_statement.consequence);
  Type t_alternative = typecheck(tenv, ast->if_statement.alternative);

  if (is_boolean(t_condition)) {
    Type lb = lower_bound(t_consequence, t_alternative);
    if (lb.type == TY_UNDEFINED) {
      SET_ERROR(ast, "incompatible types of 'then' and 'else' in 'if' expression");
    }
    return lb;
  } else {
    SET_ERROR(ast->if_statement.condition, "\"if\" condition must be of type boolean");
    return TYPE_UNDEFINED;
  }
}

Type typecheck_binary_expression(Tenv *tenv, AST *ast) {
  Type t_left = typecheck(tenv, ast->binary_expression.left);
  Type t_right = typecheck(tenv, ast->binary_expression.right);

  switch (ast->binary_expression.operator) {
  case O_GT:
  case O_LT:
  case O_EQ:
  case O_NE:
    if (is_number(t_left) && is_number(t_right)) {
      return TYPE_BOOLEAN;
    } else {
      SET_ERROR(ast, "use of comparison operator with non-numeric argument");
      return TYPE_UNDEFINED;
    }
  case O_PL:
  case O_MI:
  case O_MU:
  case O_DI:
    if (is_number(t_left) && is_number(t_right)) {
      return TYPE_NUMBER;
    } else {
      SET_ERROR(ast, "use of arithmetic operator with non-numeric argument");
      return TYPE_UNDEFINED;
    }
  case O_UNDEFINED:
    SET_ERROR(ast, "internal error: undefined operator");
    return TYPE_UNDEFINED;
  }
}

Type lower_bound(Type s, Type t) {
  if (s.type == TY_BOOLEAN && t.type == TY_BOOLEAN) { return TYPE_BOOLEAN; }
  if (s.type == TY_NUMBER && t.type == TY_NUMBER) { return TYPE_NUMBER; }
  return TYPE_UNDEFINED;
}

Type typecheck_expression_statement(Tenv *tenv, AST *ast) {
  Type ty = typecheck(tenv, ast->expression);
  return type_is_undefined(ty) ? ty : TYPE_UNIT;
}

Type typecheck_assignment(Tenv *tenv, AST *ast) {
  Type et = typecheck(tenv, ast->assignment.expression);
  
  if (type_is_undefined(et)) {
    return TYPE_UNDEFINED;
  } else {
    tenv_push(tenv, ast->assignment.identifier, et);
    return TYPE_UNIT;
  }
}

Type typecheck_sequence(Tenv *tenv, AST *ast) {
  AST_SEQUENCE *seq = ast->sequence;
  Type ty = TYPE_UNDEFINED;
  while (seq) {
    ty = typecheck(tenv, seq->statement);
    if (type_is_undefined(ty)) {
      break;
    }
    seq = seq->next;
  }
  return ty;
}

void print_type(Type t) {
  switch (t.type) {
  case TY_NUMBER:
    printf("NUMBER");
    break;
  case TY_BOOLEAN:
    printf("BOOLEAN");
    break;
  case TY_UNIT:
    printf("UNIT");
    break;
  case TY_UNDEFINED:
    printf("UNDEFINED");
    break;
  }
}

int type_is_undefined(Type t) {
  return t.type == TY_UNDEFINED;
}

int is_number(Type t) {
  return t.type == TY_NUMBER;
}

int is_boolean(Type t) {
  return t.type == TY_BOOLEAN;
}

Tenv *tenv_new(int capacity) {
  Tenv *tenv = malloc(sizeof(Tenv));
  tenv->entries = malloc(sizeof(TenvEntry) * capacity);
  tenv->size = 0;
  tenv->capacity = capacity;
  return tenv;
}

int tenv_push(Tenv *tenv, const char *identifier, Type type) {
  if (tenv->size == tenv->capacity) {
    return 0;
  }
  tenv->entries[tenv->size].identifier = identifier;
  tenv->entries[tenv->size].type = type;
  tenv->size++;
  return 1;
}

Type tenv_lookup(Tenv *tenv, const char *identifier) {
  for (int i = tenv->size - 1; i >= 0; i++) {
    if (strcmp(tenv->entries[i].identifier, identifier) == 0) {
      return tenv->entries[i].type;
    }
  }
  return TYPE_UNDEFINED;
}

int tenv_pop(Tenv *tenv) {
  if (tenv->size == 0) {
    return 0;
  }
  tenv->size--;
  return 1;
}

void tenv_free(Tenv *tenv) {
  free(tenv->entries);
  free(tenv);
}

void tenv_print(Tenv *tenv) {
  printf("Tenv {\n");
  for (int i = 0; i < tenv->size; i++) {
    printf("  %s: ", tenv->entries[i].identifier);
    print_type(tenv->entries[i].type);
    if (i < tenv->size - 1) {
      printf("; ");
    }
  }
  printf("%s}\n", tenv->size == 0 ? "}" : "\n");
}