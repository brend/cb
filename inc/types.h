#ifndef __TYPES_H__
#define __TYPES_H__

#include "parser.h"

typedef enum {
  TY_UNDEFINED,
  TY_NUMBER,
  TY_BOOLEAN
} TypeT;

typedef struct {
  TypeT type;
} Type;

typedef struct {
} Tenv;

Type typecheck(AST *ast);

void print_type(Type t);

#endif
