#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "lexer.h"
#include "io.h"

void evaluate_expression(char *expression);
void evaluate_file(char *filename);

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("usage: %s <file>\n       %s -e <expression>\n", argv[0], argv[0]);
		return -1;
	}

	if (strcmp(argv[1], "-e") == 0 && argc >= 3) {
		char *expression = argv[2];
		evaluate_expression(expression);
		return 0;
	}

	char *filename = argv[1];
	evaluate_file(filename);
}

void evaluate_expression(char *expression) {
    printf("=== input ===\n");
    printf("%s\n", expression);

	printf("=== lexical tokens ===\n");
	print_tokens_from_expression(expression);

	printf("=== abstract syntax tree ===\n");
}

void evaluate_file(char *filename) {
	printf("=== input ===\n");
	print_file(filename);

  	printf("=== lexical tokens ===\n");
  	print_tokens_from_file(filename);

  	printf("=== abstract syntax tree ===\n");
	AST *ast = parse_file(filename);

	print_ast(ast);
	printf("\n");
}