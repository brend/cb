#include "interpreter.h"
#include <stdlib.h>

Value evaluate(AST *ast) {
  Value v = {0};

  switch (ast->type) {
  case AST_NUMBER:
    v.intValue = ast->number;
    break;
  case AST_SYMBOL:
    v.intValue = 0;
    break;
  case AST_IF:
    v = evaluate(ast->if_statement.condition);
    AST *exp = v.intValue ? ast->if_statement.consequence : ast->if_statement.alternative;
    v = evaluate(exp);
    break;
  default:
    fprintf(stderr, "Unknown expression type: %d\n", ast->type);
    exit(-2);
  }

  return v;
}
