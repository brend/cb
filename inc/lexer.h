#ifndef __LEXER_H__
#define __LEXER_H__

#include "stream.h"

typedef enum {
	T_IV,
	T_PL,
	T_MI,
	T_GT,
	T_LT,
	T_LP,
	T_RP,
	T_ID,
	T_IF
} token_type;

typedef struct {
	token_type type;
	char text[8];
} token;

typedef struct {
	stream input;
} lexer;

lexer lexer_from_file(const char *filename);
int lexer_close(lexer);

token lexer_peek(lexer*);
token lexer_pop(lexer*);

int token_is_invalid(token);

#endif