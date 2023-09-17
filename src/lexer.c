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

#define MATCHW(w, y) { \
	if (strcmp(id_buffer, (w)) == 0) { \
		strcpy(t.text, id_buffer); \
		t.type = (y); \
		return t; \
	} \
}

token lexer_peek(lexer *lexer) {
	token t = token_new();
	const size_t tlen = sizeof(t.text);
	stream *s = &lexer->input;
	
	stream_consume_whitespace(s);

	t.line = s->line;
	t.column = s->column;
	
	MATCH("+", T_PL);
	MATCH("-", T_MI);
	MATCH(">", T_GT);
	MATCH("<", T_LT);
	MATCH("(", T_LP);
	MATCH(")", T_RP);
	
	if (!stream_consume_alphanum_prefix(s, id_buffer, sizeof(id_buffer))) {
		return t;
	}
	
	MATCHW("if", T_IF);
	MATCHW("then", T_TN);
	MATCHW("else", T_EL);

	char *num_end;
	long number = strtol(id_buffer, &num_end, 10);

	if (*num_end == '\0') {
		t.type = T_NU;
		strcpy(t.text, id_buffer);
		return t;
	}
	
	t.type = T_ID;
	strcpy(t.text, id_buffer);

	return t;
}

token lexer_pop(lexer *lexer) {
	return token_new();
}

int token_is_invalid(token t) {
	return t.type == T_IV;
}
