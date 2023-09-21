#ifndef __LEXER_H__
#define __LEXER_H__

#include "stream.h"
#include "queue.h"

typedef enum {
	T_IV,
	T_PL,
	T_MI,
	T_GT,
	T_LT,
	T_LP,
	T_RP,
	T_ID,
	T_NU,
	T_IF,
	T_TN,
	T_EL
} token_type;

typedef struct {
	token_type type;
	char text[8];
	int line;
	int column;
} token;

typedef struct {
	Stream *input;
	Queue *buffer;
} lexer;

lexer *lexer_from_file(const char *filename);
lexer *lexer_from_expression(const char *expression);
int lexer_close(lexer*);

token *lexer_peek(lexer*);
token *lexer_pop(lexer*);

int token_is_invalid(token*);

#endif
