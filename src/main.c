#include <stdio.h>
#include "parser.h"
#include "io.h"
#include "interpreter.h"
#include "opt.h"
#include "aux.h"

int evaluate_file(FILE *file);
void show_usage(void);
void show_help(void);
int run_nl(Options *opt);

int main(int argc, char** argv) {
  Options opt = opt_get(argc, argv);

  int result = run_nl(&opt);

  opt_destroy(&opt);

  return result;
}

int run_nl(Options *opt) {
  if (opt_invalid(opt)) {
    show_usage();
    return -1;
  }

  FILE *input = NULL;

  switch (opt->mode) {
  case PM_Show_Help:
    show_help();
    return 0;
  case PM_Eval_Expr:
    input = file_from_string(opt->expression);
    break;
  case PM_Eval_File:
    input = fopen(opt->filename, "r");
    break;
  case PM_Eval_Stdin:
    opt->expression = read_stdin_to_end();
    input = file_from_string(opt->expression);
    break;
  default:
    fprintf(stderr, "internal error: invalid mode: %d\n", opt->mode);
    return -3;
  }

  if (!input) {
    return -2;
  }

  return evaluate_file(input);
}

int evaluate_file(FILE *file) {
	printf("=== input ===\n");
	print_file(file);
	
	fseek(file, 0, SEEK_SET);

  	printf("=== lexical tokens ===\n");
  	print_tokens_from_file(file);

	fseek(file, 0, SEEK_SET);

  	printf("=== abstract syntax tree ===\n");
	AST *ast = parse_file(file);

	print_ast(ast);
	printf("\n");

	printf("=== evaluation ===\n");
	if (ast) {
		Value v = evaluate(ast);

		printf("%ld\n", v.intValue);
	}

	int ok = ast_destroy(&ast);

	printf("ast destroy: %d\n", ok);

  return 0;
}

void show_usage(void) {
  printf("usage: %s \n", "nl");
}

void show_help(void) {
  printf("help!");
}
