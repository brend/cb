#include <stdio.h>
#include "parser.h"
#include "lexer.h"

void print_file(char *filename) {
  char buffer[1024*1024];
  FILE *f = fopen(filename, "r");
  fread(buffer, sizeof(char), sizeof(buffer), f);
  printf("%s\n", buffer);
  fclose(f);
}

void print_token(token *t) {
  if (!t) { printf("NULL"); return; }
		printf("%s\t", t->text);
				
		switch (t->type) {
			case T_PL:
			printf("PLUS");
			break;
			case T_MI:
			printf("MINUS");
			break;
			case T_GT:
			printf("GT");
			break;
			case T_LT:
			printf("LT");
			break;
			case T_IF:
			printf("IF");
			break;
			case T_TN:
			printf("THEN");
			break;
			case T_EL:
			printf("ELSE");
			break;
			case T_ID:
			printf("ID");
			break;
			case T_NU:
			printf("NUMBER");
			break;
			default:
			printf("EOF");
			break;			
		}

		printf("\t%d:%d", t->line+1, t->column+1);
}

void print_tokens(char *filename) {
  lexer *lexer = lexer_from_file(filename);
  token *token;

  while ((token = lexer_pop(lexer)) && token->type != T_IV) {
    print_token(token);
    printf("\n");
  }
}

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("usage: %s <file>\n", argv[0]);
		return -1;
	}

  printf("=== input ===\n");
  print_file(argv[1]);

  printf("=== lexical tokens ===\n");
  print_tokens(argv[1]);

  printf("=== abstract syntax tree ===\n");
	AST *ast = parse_file(argv[1]);

	print_ast(ast);
	printf("\n");

}
