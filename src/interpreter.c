#include "interpreter.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Value evaluate(Venv *venv, AST *ast) {
  Value v = {0};
  Value w = {0};
  AST_SEQUENCE *seq = NULL;

  switch (ast->type) {
  case AST_NUMBER:
    v.intValue = ast->number;
    break;
  case AST_SYMBOL:
    v.intValue = venv_lookup(venv, ast->symbol).intValue;
    break;
  case AST_IF:
    v = evaluate(venv, ast->if_statement.condition);
    AST *exp = v.intValue ? ast->if_statement.consequence : ast->if_statement.alternative;
    v = evaluate(venv, exp);
    break;
  case AST_BINARY:
    v = evaluate(venv, ast->binary_expression.left);
    w = evaluate(venv, ast->binary_expression.right);
    switch (ast->binary_expression.operator) {
    case O_PL:
      v.intValue = v.intValue + w.intValue;
      break;
    case O_MI:
      v.intValue = v.intValue - w.intValue;
      break;
    case O_MU:
      v.intValue = v.intValue * w.intValue;
      break;
    case O_DI:
            if (w.intValue == 0) {
                fprintf(stderr, "evaluation error: division by zero\n");
                exit(300);
            }
      v.intValue = v.intValue / w.intValue;
      break;
    case O_GT:
      v.intValue = (v.intValue > w.intValue) ? 1 : 0;
      break;
    case O_LT: 
      v.intValue = (v.intValue < w.intValue) ? 1 : 0;
      break;
    case O_EQ:
      v.intValue = (v.intValue == w.intValue) ? 1 : 0;
      break;
    case O_NE:
      v.intValue = (v.intValue != w.intValue) ? 1 : 0;
      break;
    case O_UNDEFINED:
      fprintf(stderr, "internal error: undefined operator\n");
      exit(-3);
    }
    break;
  case AST_STMT_EXP:
    v = evaluate(venv, ast->expression);
    break;
  case AST_STMT_ASN:
    v = evaluate(venv, ast->assignment.expression);
    venv_push(venv, ast->assignment.identifier, v);
    break;
  case AST_STMT_SEQ:
    seq = ast->sequence;
    while (seq) {
      v = evaluate(venv, seq->statement);
      seq = seq->next;
    }
    break;
  case AST_UNDEFINED:
    fprintf(stderr, "internal error: undefined ast type\n");
    exit(-4);
  }

  return v;
}

Venv *venv_new(int capacity) {
  Venv *venv = malloc(sizeof(Venv));
  venv->entries = malloc(sizeof(VenvEntry) * capacity);
  venv->size = 0;
  venv->capacity = capacity;
  return venv;
}

int venv_push(Venv *venv, const char *identifier, Value value) {
  if (venv->size == venv->capacity) {
    return 0;
  }
  venv->entries[venv->size].identifier = identifier;
  venv->entries[venv->size].value = value;
  venv->size++;
  return 1;
}

Value venv_lookup(Venv *venv, const char *identifier) {
  for (int i = venv->size - 1; i >= 0; i--) {
    if (strcmp(venv->entries[i].identifier, identifier) == 0) {
      return venv->entries[i].value;
    }
  }
  return (Value){0};
}

int venv_pop(Venv *venv) {
  if (venv->size == 0) {
    return 0;
  }
  venv->size--;
  return 1;
}

void venv_destroy(Venv **venv) {
  if (!(venv && *venv)) { return; }
  free((*venv)->entries);
  free(*venv);
  *venv = NULL;
}

void print_value(Value v) {
  printf("%ld", v.intValue);
}

void venv_print(Venv *venv) {
  printf("Venv {\n");
  for (int i = 0; i < venv->size; i++) {
    printf("  %s: ", venv->entries[i].identifier);
    print_value(venv->entries[i].value);
    if (i < venv->size - 1) {
      printf("; ");
    }
  }
  printf("%s}\n", venv->size == 0 ? "" : "\n");
}