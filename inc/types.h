#ifndef __TYPES_H__
#define __TYPES_H__

#include "ast.h"

Type typecheck_ast(Tenv *tenv, AST *ast);
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