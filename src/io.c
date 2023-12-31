#include "io.h"
#include <stdio.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"

void print_file(FILE *file) {
  char buffer[1024*1024];
  memset(buffer, 0, sizeof(buffer));
  fread(buffer, sizeof(char), sizeof(buffer), file);
  printf("%s\n", buffer);
}

void print_token(Token *t) {
  if (!t) { printf("NULL"); return; }
		printf("%s\t", t->text);
				
		switch (t->type) {
			case T_PL:
			printf("PLUS");
			break;
			case T_MI:
			printf("MINUS");
			break;
      case T_MU:
      printf("MUL");
      break;
      case T_DI:
      printf("DIV");
      break;
      case T_EQ:
      printf("EQ");
      break;
      case T_NE:
      printf("NEQ");
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
      case T_LP:
      printf("LP");
      break;
      case T_RP:
      printf("RP");
      break;
      case T_VL:
      printf("VL");
      break;
			case T_IV:
			printf("INVALID");
			break;			
		}

		printf("\t%d:%d", t->line+1, t->column+1);
}

void print_tokens_from_file(FILE *file) {
  Lexer *lexer = lexer_from_file(file);
  Token token;

  while (tvalid(token = lexer_pop(lexer))) {
    print_token(&token);
    printf("\n");
  }

  lexer_destroy(&lexer);
}

void print_tokens_from_expression(char *expression) {
  Lexer *lexer = lexer_from_expression(expression);
  Token token;
  
  while (tvalid(token = lexer_pop(lexer))) {
    print_token(&token);
    printf("\n");
  }

  lexer_destroy(&lexer);
}

