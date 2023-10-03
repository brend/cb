#include "lexer.h"
#include <string.h>
#include <stdlib.h>
#include "aux.h"

#define TOKEN_BUFFER_SIZE 1024

static char ID_BUFFER[128];

int lexer_tokenize_stream(Lexer *lexer, Stream *s);

Lexer *lexer_new(Stream *input)
{
  Lexer *lexer = malloc(sizeof(Lexer));
  memset(lexer, 0, sizeof(Lexer));
  lexer->capacity = TOKEN_BUFFER_SIZE;
  lexer->tokens = malloc(sizeof(Token) * lexer->capacity);
  memset(lexer->tokens, 0, sizeof(Token) * lexer->capacity);
  lexer->token_count = 0;
  lexer->current_token = 0;

  int tokenize_success = lexer_tokenize_stream(lexer, input);

  stream_destroy(&input);

  if (tokenize_success)
  {
    return lexer;
  }
  else
  {
    lexer_destroy(&lexer);
    return NULL;
  }
}

Lexer *lexer_open_file(const char *filename)
{
  if (!filename) { return NULL; }
  return lexer_new(stream_open_file(filename));
}

Lexer *lexer_from_file(FILE *file)
{
  if (!file) { return NULL; }
  return lexer_new(stream_from_file(file));
}

Lexer *lexer_from_expression(const char *expression)
{
  if (!expression) { return NULL; }
  return lexer_new(stream_from_string(expression));
}

int lexer_destroy(Lexer **lexer)
{
  if (!(lexer && *lexer))
  {
    return 0;
  }

  if ((*lexer)->tokens)
  {
    free((*lexer)->tokens);
    (*lexer)->tokens = NULL;
  }

  free(*lexer);
  *lexer = NULL;

  return 1;
}

#define MATCH(w, y)                            \
  {                                            \
    if (stream_consume(s, (w)))                \
    {                                          \
      t.type = (y);                            \
      strcpy(t.text, (w));                     \
      lexer->tokens[lexer->token_count++] = t; \
      continue;                                \
    }                                          \
  }

#define MATCHW(w, y)                           \
  {                                            \
    if (strcmp(ID_BUFFER, (w)) == 0)           \
    {                                          \
      t.type = (y);                            \
      lexer->tokens[lexer->token_count++] = t; \
      continue;                                \
    }                                          \
  }

int lexer_tokenize_stream(Lexer *lexer, Stream *s)
{
  while (stream_peek_char(s, NULL))
  {
    if (lexer->current_token >= lexer->capacity)
    {
      lexer->capacity += TOKEN_BUFFER_SIZE;
      lexer->tokens = realloc(lexer->tokens, sizeof(Token) * lexer->capacity);
    }

    if (!stream_consume_whitespace(s))
    {
      return 0;
    }

    Token t = {0};
    t.line = s->line;
    t.column = s->column;

    MATCH("+", T_PL);
    MATCH("-", T_MI);
    MATCH("*", T_MU);
    MATCH("/", T_DI);
    MATCH("=", T_EQ);
    MATCH("<>", T_NE);
    MATCH(">", T_GT);
    MATCH("<", T_LT);
    MATCH("(", T_LP);
    MATCH(")", T_RP);

    if (!stream_consume_alphanum_prefix(s, ID_BUFFER, sizeof(ID_BUFFER)))
    {
      return 0;
    }

    MATCHW("if", T_IF);
    MATCHW("then", T_TN);
    MATCHW("else", T_EL);
    MATCHW("val", T_VL);

    strcpy(t.text, ID_BUFFER);

    if (ascii_to_long(ID_BUFFER, NULL))
    {
      t.type = T_NU;
    }
    else
    {
      t.type = T_ID;
    }

    lexer->tokens[lexer->token_count++] = t;
  }

  return 1;
}

Token lexer_peek(Lexer *lexer)
{
  if (lexer &&
      lexer->current_token >= 0 &&
      lexer->current_token < lexer->token_count)
  {
    return lexer->tokens[lexer->current_token];
  }
  else
  {
    return (Token){0};
  }
}

Token lexer_pop(Lexer *lexer)
{
  if (lexer &&
      lexer->current_token >= 0 &&
      lexer->current_token < lexer->token_count)
  {
    return lexer->tokens[lexer->current_token++];
  }
  else
  {
    return (Token){0};
  }
}

inline int tvalid(Token t) {
  return t.type != T_IV;
}

inline int teq(Token token, TokenType type) {
  return token.type == type;
}