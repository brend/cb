#include "io.h"
#include <stdio.h>
#include "lexer.h"
#include "parser.h"

void print_file(FILE *file) {
  char buffer[1024*1024];
  fread(buffer, sizeof(char), sizeof(buffer), file);
  printf("%s\n", buffer);
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

void print_tokens_from_file(FILE *file) {
  lexer *lexer = lexer_from_file(file);
  token *token;

  while ((token = lexer_pop(lexer)) && token->type != T_IV) {
    print_token(token);
    printf("\n");
  }

  // TODO: destroy lexer
}

void print_tokens_from_expression(char *expression) {
    lexer *lexer = lexer_from_expression(expression);
    token *token;
    
    while ((token = lexer_pop(lexer)) && token->type != T_IV) {
        print_token(token);
        printf("\n");
    }

    // TODO: destroy lexer
}