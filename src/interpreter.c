#include "interpreter.h"
#include <stdlib.h>

Value evaluate(AST *ast) {
  Value v = {0};
  Value w = {0};

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
  case AST_BINARY:
    v = evaluate(ast->binary_expression.left);
    w = evaluate(ast->binary_expression.right);
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
    case O_UNDEFINED:
      fprintf(stderr, "internal error: undefined operator\n");
      exit(-3);
    }
    break;
  default:
    fprintf(stderr, "Unknown expression type: %d\n", ast->type);
    exit(-2);
  }

  return v;
}
