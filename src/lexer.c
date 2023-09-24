#include "lexer.h"
#include <string.h>
#include <stdlib.h>
#include "aux.h"

#define TOKEN_BUFFER_SIZE 64

static char ID_BUFFER[128];

lexer *lexer_new(Stream *input) {
  lexer *lex = malloc(sizeof(lexer));
  memset(lex, 0, sizeof(lexer));
  lex->input = input;
  lex->buffer = queue_new(TOKEN_BUFFER_SIZE);
  return lex;
}

lexer *lexer_open_file(const char *filename) {
  if (!filename) { return NULL; }
  return lexer_new(stream_open_file(filename));
}

lexer *lexer_from_file(FILE *file) {
  if (!file) { return NULL; }
  return lexer_new(stream_from_file(file));
}

lexer *lexer_from_expression(const char *expression) {
  if (!expression) { return NULL; }
  return lexer_new(stream_from_string(expression));
}

int lexer_destroy(lexer **lexer) {
  if (!(lexer && *lexer)) {
    return 0;
  }

  if ((*lexer)->input) {
    if (!stream_destroy(&(*lexer)->input)) { return 0; }
    (*lexer)->input = NULL;
  }

  if ((*lexer)->buffer) {
    token *t = NULL;
    while ((t = queue_dequeue_ptr((*lexer)->buffer))) { if (t) { token_destroy(t); } }
    if (!queue_destroy(&(*lexer)->buffer)) { return 0; }
    (*lexer)->buffer = NULL;
  }

  free(*lexer);
  *lexer = NULL;

  return 1;
}

token *token_new() {
  token *t = malloc(sizeof(token));
  memset(t, 0, sizeof(token));
  return t;
}

int token_destroy(token *t) {
  if (t) {
    free(t);
  }
  return 1;
}

#define MATCH(w, y) { \
  if (stream_consume(s, (w))) { \
    t->type = (y); \
    queue_enqueue_ptr(lexer->buffer, t); \
    return t; \
  } \
}

#define MATCHW(w, y) { \
  if (strcmp(ID_BUFFER, (w)) == 0) { \
    t->type = (y); \
    queue_enqueue_ptr(lexer->buffer, t); \
    return t; \
  } \
}

token *lexer_peek(lexer *lexer) {
  if (!lexer) { return NULL; }

  token *t = NULL;

  if (!queue_is_empty(lexer->buffer)) {
    queue_peek_ptr(lexer->buffer, (void**)&t);
    return t;
  }

  Stream *s = lexer->input;
  
  if (!stream_consume_whitespace(s)) {
    return NULL;
  }

  t = token_new();
  t->line = s->line;
  t->column = s->column;
  
  MATCH("+", T_PL);
  MATCH("-", T_MI);
  MATCH("*", T_MU);
  MATCH("/", T_DI);
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

  strcpy(t->text, ID_BUFFER);
  queue_enqueue_ptr(lexer->buffer, t);

  if (ascii_to_long(ID_BUFFER, NULL)) {
    t->type = T_NU;
  } else {
    t->type = T_ID;
  }

  return t;
}

token *lexer_pop(lexer *lexer) {
  if (!lexer) { return NULL; }
  token *t = lexer_peek(lexer);
  if (t) {
    queue_dequeue(lexer->buffer);
  }
  return t;
}

int token_is_invalid(token *t) {
  return t == NULL || t->type == T_IV;
}
