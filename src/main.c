#include <stdio.h>
#include "lexer.h"

int main() {
	lexer lex = lexer_from_file("data/sample.nl");
	token t;
	
	while (1) {
		t = lexer_peek(&lex);
				
		switch (t.type) {
			case T_PL:
			printf("<PLUS:");
			break;
			case T_MI:
			printf("<MINUS:");
			break;
			case T_GT:
			printf("<GT:");
			break;
			case T_LT:
			printf("<LT:");
			break;
			case T_IF:
			printf("<IF:");
			break;
			case T_ID:
			printf("<ID:");
			break;
			default:
			printf("<???:");
			break;			
		}
		
		printf("%s>\n", t.text);
		
		if (token_is_invalid(t)) {
			printf("that's it\n");
			break;
		}
	}
	
	lexer_close(lex);
	
	return 0;
}