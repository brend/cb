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
} Tenv;

Type typecheck(AST *ast);
const char *typecheck_last_error();
void print_type(Type t);
int type_is_undefined(Type t);

#endif
