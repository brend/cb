#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__

#include "parser.h"

typedef struct {
  long intValue;
} Value;

Value evaluate(AST *ast);

#endif
