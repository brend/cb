#include "lexer.h"
#include <string.h>
#include <stdlib.h>

lexer lexer_from_file(const char *filename) {
	lexer lex;
	
	lex.input = stream_open_file(filename);
	
	return lex;
}

int lexer_close(lexer lexer) {
		return stream_close(&lexer.input);
}

token token_new() {
	token t;
	
	memset(&t, 0, sizeof(token));
	
	return t;
}

int char_is_whitespace(char c) {
	switch (c) {
		case ' ':
		case '\t':
		case '\r':
		case '\n':
		return 1;
		default:
		return 0;
	}
}

char id_buffer[64];

#define MATCH(w, y) { \
	if (stream_consume(s, (w))) { \
		strcpy(t.text, (w)); \
		t.type = (y); \
		return t; \
	} \
}

token lexer_peek(lexer *lexer) {
	token t = token_new();
	const size_t tlen = sizeof(t.text);
	stream *s = &lexer->input;
	
	stream_consume_whitespace(s);
	
	MATCH("+", T_PL);
	MATCH("-", T_MI);
	MATCH(">", T_GT);
	MATCH("<", T_LT);
	MATCH("(", T_LP);
	MATCH(")", T_RP);
	
	if (!stream_consume_alphanum_prefix(s, id_buffer, sizeof(id_buffer))) {
		return t;
	}
	
	if (strcmp(id_buffer, "if") == 0) {
		t.type = T_IF;
	} else {
		t.type = T_ID;
	}
	
	if (strlen(id_buffer) > tlen) {
		printf("ERROR\ttext too long for token storage: %s\n", id_buffer);
		exit(1);
	}
	
	strcpy(t.text, id_buffer);
	
	return t;
}

token lexer_pop(lexer *lexer) {
	return token_new();
}

int token_is_invalid(token t) {
	return t.type == T_IV;
}