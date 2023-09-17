#include <stdio.h>
#include "parser.h"

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("usage: %s <file>\n", argv[0]);
		return -1;
	}

	AST *ast = parse_file(argv[1]);

	print_ast(ast);
	printf("\n");
	
	return 0;
}

/*
lexer lex = lexer_from_file(argv[1]);
	token t;
	
	while (1) {
		t = lexer_peek(&lex);

		printf("%s\t", t.text);
				
		switch (t.type) {
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

		printf("\t%d:%d\n", t.line+1, t.column+1);
		
		if (token_is_invalid(t)) {
			printf("that's it\n");
			break;
		}
	}
	
	lexer_close(lex);
	*/