#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__

#include "parser.h"

typedef struct {
  long intValue;
} Value;

typedef struct {
  const char *identifier;
  Value value;
} VenvEntry;

typedef struct {
  VenvEntry *entries;
  int size;
  int capacity;
} Venv;

void print_value(Value v);

Value evaluate(Venv *venv, AST *ast);

Venv *venv_new(int capacity);
int venv_push(Venv *venv, const char *identifier, Value value);
Value venv_lookup(Venv *venv, const char *identifier);
int venv_pop(Venv *venv);
void venv_destroy(Venv **venv);
void venv_print(Venv *venv);

#endif
