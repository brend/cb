#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "lexer.h"
#include "io.h"
#include "aux.h"
#include "interpreter.h"
#include "opt.h"

void evaluate_file_ptr(FILE *file);

int main(int argc, char** argv) {
  Options opt = opt_get(argc, argv);

  if (!opt.input) {
		printf("usage: %s <file>\n       %s -e <expression>\n", argv[0], argv[0]);
		return -1;
	}

	evaluate_file_ptr(opt.input);
  opt_destroy(&opt);
}

void evaluate_file_ptr(FILE *file) {
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
}
