#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "lexer.h"
#include "io.h"
#include "aux.h"
#include "interpreter.h"

void evaluate_expression(char *expression);
void evaluate_file(char *filename);
void evaluate_file_ptr(FILE *file);
char *read_stdin_to_end();

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
    char *expression = read_stdin_to_end();
	evaluate_expression(expression);
    free(expression);
		return 0;
	}

	char *filename = argv[1];
	evaluate_file(filename);
}


char *read_stdin_to_end() {
  const size_t CHUNK_SIZE = 1024;
  char *buffer = NULL;
  size_t buffer_size = 0;
  size_t bytes_read = 0;
  size_t total_bytes_read = 0;

  do {
    buffer_size += CHUNK_SIZE;
    buffer = realloc(buffer, buffer_size);
    bytes_read = fread(buffer + total_bytes_read, 1, CHUNK_SIZE, stdin);
    total_bytes_read += bytes_read;
  } while (bytes_read == CHUNK_SIZE);

  return buffer;
}

void evaluate_expression(char *expression) {
  FILE *file = file_from_string(expression);
  evaluate_file_ptr(file);
  fclose(file);
}

void evaluate_file(char *filename) {
	FILE *file = fopen(filename, "r");
	evaluate_file_ptr(file);
	fclose(file);
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

	ast_destroy(&ast);
}
