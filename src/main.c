#include <stdio.h>
#include "parser.h"
#include "io.h"
#include "interpreter.h"
#include "opt.h"
#include "aux.h"
#include "types.h"

int evaluate_file(Options *opt, FILE *file);
void show_usage(void);
void show_help(void);
int run_cb(Options *opt);

int main(int argc, char** argv) {
  Options opt = opt_get(argc, argv);

  int result = run_cb(&opt);

  opt_destroy(&opt);

  return result;
}

int run_cb(Options *opt) {
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

  return evaluate_file(opt, input);
}

int evaluate_file(Options *opt, FILE *file) {
  if (opt->verbose) {
	  printf("=== input ===\n");
	  print_file(file);
	  fseek(file, 0, SEEK_SET);
  	printf("=== lexical tokens ===\n");
  	print_tokens_from_file(file);
	  fseek(file, 0, SEEK_SET);
  	printf("=== abstract syntax tree ===\n");
  }

	AST *ast = parse_file(file);

  if (opt->verbose) {
	  print_ast(ast);
	  printf("\n");
    printf("=== typecheck ===\n");
  }

  Tenv *tenv = tenv_new(100);
  Type type = typecheck(tenv, ast);
  tenv_print(tenv);
  const char *type_error = typecheck_last_error();

  if (type_error) {
    fprintf(stderr, "%s\n", type_error);
  }  

  if (opt->verbose) {
    print_type(type);
    printf("\n");
  }

  if (type_is_undefined(type)) {
    ast_destroy(&ast);
    return 800;
  }

  tenv_free(tenv);
  tenv = NULL;

  Value v = evaluate(ast);

	if (opt->verbose) {
    printf("=== evaluation ===\n");
  }

	printf("%ld\n", v.intValue);

	ast_destroy(&ast);

  return 0;
}

void show_usage(void) {
  printf("usage: %s \n", "cb");
}

void show_help(void) {
  printf("help will be coming soon!\n");
}
