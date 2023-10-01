#ifndef __TYPES_H__
#define __TYPES_H__

#include "parser.h"

typedef enum {
  TY_UNDEFINED,
  TY_NUMBER,
  TY_BOOLEAN,
  TY_UNIT
} TypeT;

typedef struct {
  TypeT type;
} Type;

typedef struct {
  const char *identifier;
  Type type;
} TenvEntry;

typedef struct {
  TenvEntry *entries;
  int size;
  int capacity;
} Tenv;

Type typecheck(Tenv *tenv, AST *ast);
const char *typecheck_last_error();
void print_type(Type t);
int type_is_undefined(Type t);

Tenv *tenv_new(int capacity);
int tenv_push(Tenv *tenv, const char *identifier, Type type);
Type tenv_lookup(Tenv *tenv, const char *identifier);
int tenv_pop(Tenv *tenv);
void tenv_destroy(Tenv **tenv);
void tenv_print(Tenv *tenv);

#endif