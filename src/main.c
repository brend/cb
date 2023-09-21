#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "lexer.h"
#include "io.h"

void evaluate_expression(char *expression);
void evaluate_file(char *filename);
void evaluate_file_ptr(FILE *file);
void evaluate_stdin();

int main(int argc, char** argv) {
	if (argc < 1) {
		printf("usage: %s <file>\n       %s -e <expression>\n", argv[0], argv[0]);
		return -1;
	}

	if (argc >= 3 && strcmp(argv[1], "-e") == 0) {
		char *expression = argv[2];
		evaluate_expression(expression);
		return 0;
	}

	if (argc == 1) {
		evaluate_stdin();
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
	FILE *file = fopen(filename, "r");
	evaluate_file_ptr(file);
	fclose(file);
}

void evaluate_stdin() {
	evaluate_file_ptr(stdin);
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
}