#ifndef __LEXER_H__
#define __LEXER_H__

#include <stdio.h>
#include "stream.h"

typedef enum {
	T_IV,
	T_PL,
	T_MI,
    T_MU,
    T_DI,
    T_EQ,
    T_NE,
	T_GT,
	T_LT,
	T_LP,
	T_RP,
	T_ID,
	T_NU,
	T_IF,
	T_TN,
	T_EL,
  T_VL
} TokenType;

typedef struct {
	TokenType type;
	char text[64];
	int line;
	int column;
} Token;

typedef struct {
	int capacity;
	Token *tokens;
	int token_count;
	int current_token;
} Lexer;

int tvalid(Token);

Lexer *lexer_open_file(const char *filename);
Lexer *lexer_from_file(FILE *file);
Lexer *lexer_from_expression(const char *expression);

int lexer_destroy(Lexer**);

Token lexer_peek(Lexer*);
Token lexer_pop(Lexer*);

#endif
