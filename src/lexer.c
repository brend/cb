#include "lexer.h"
#include <string.h>
#include <stdlib.h>
#include "aux.h"

lexer *lexer_from_file(const char *filename) {
  lexer *lex = malloc(sizeof(lexer));
  lex->input = stream_open_file(filename);
  lex->buffer = queue_new(64);
  return lex;
}

int lexer_close(lexer *lexer) {
  // TODO: free lexer buffer queue
  return stream_close(lexer->input);
}

token *token_new() {
token *t = malloc(sizeof(token));
  memset(t, 0, sizeof(token));
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

char ID_BUFFER[64];

#define MATCH(w, y) { \
  if (stream_consume(s, (w))) { \
    strcpy(t->text, (w)); \
    t->type = (y); \
    queue_enqueue_ptr(lexer->buffer, t); \
    return t; \
  } \
}

#define MATCHW(w, y) { \
  if (strcmp(ID_BUFFER, (w)) == 0) { \
    strcpy(t->text, ID_BUFFER); \
    t->type = (y); \
    queue_enqueue_ptr(lexer->buffer, t); \
    return t; \
  } \
}

token *lexer_peek(lexer *lexer) {
  token *t = NULL;

  if (!queue_is_empty(lexer->buffer)) {
    queue_peek_ptr(lexer->buffer, (void**)&t);
    return t;
  }

  t = token_new();
  Stream *s = lexer->input;
  
  stream_consume_whitespace(s);

  t->line = s->line;
  t->column = s->column;
  
  MATCH("+", T_PL);
  MATCH("-", T_MI);
  MATCH(">", T_GT);
  MATCH("<", T_LT);
  MATCH("(", T_LP);
  MATCH(")", T_RP);
  
  if (!stream_consume_alphanum_prefix(s, ID_BUFFER, sizeof(ID_BUFFER))) {
    return t;
  }
  
  MATCHW("if", T_IF);
  MATCHW("then", T_TN);
  MATCHW("else", T_EL);

  if (ascii_to_long(ID_BUFFER, NULL)) {
    t->type = T_NU;
    strcpy(t->text, ID_BUFFER);
    queue_enqueue_ptr(lexer->buffer, t);
    return t;
  }
  
  t->type = T_ID;
  strcpy(t->text, ID_BUFFER);

  queue_enqueue_ptr(lexer->buffer, t);

  return t;
}

token *lexer_pop(lexer *lexer) {
  token *t = lexer_peek(lexer);
  if (t) {
    queue_dequeue(lexer->buffer);
  }
  return t;
}

int token_is_invalid(token *t) {
  return t == NULL || t->type == T_IV;
}
