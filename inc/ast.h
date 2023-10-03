#ifndef __AST_H__
#define __AST_H__

/**
 * TYPES
*/
typedef enum {
  TY_UNDEFINED,
  TY_NUMBER,
  TY_BOOLEAN,
  TY_UNIT
} TypeT;

typedef struct {
  TypeT type;
} Type;

typedef struct {
  const char *identifier;
  Type type;
} TenvEntry;

typedef struct {
  TenvEntry *entries;
  int size;
  int capacity;
} Tenv;

/**
 * LEXICAL ANALYSIS
*/

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

/**
 * ABSTRACT SYNTAX TREE
*/

typedef enum {
  AST_UNDEFINED,
  AST_NUMBER,
  AST_SYMBOL,
  AST_IF,
	AST_BINARY,

  AST_STMT_EXP,
  AST_STMT_ASN,
  AST_STMT_SEQ
} AST_TYPE;

struct AST;

typedef struct {
    struct AST *condition;
    struct AST *consequence;
    struct AST *alternative;
} AST_IF_STATEMENT;

typedef enum {
  O_UNDEFINED,
  O_PL,
  O_MI,
  O_MU,
  O_DI,
  O_EQ,
  O_NE,
  O_GT,
  O_LT
} Operator;

typedef struct {
  Operator operator;
  struct AST *left;
  struct AST *right;
} AST_BINARY_EXPRESSION;

typedef struct {
  char identifier[64];
  struct AST *expression;
} AST_ASSIGNMENT;

typedef struct AST_SEQUENCE {
  struct AST *statement;
  struct AST_SEQUENCE *next;
} AST_SEQUENCE;

typedef struct AST {
    AST_TYPE type;
    Token first_token;
    Token last_token;
    union {
      long number;
      char symbol[64];
      AST_IF_STATEMENT if_statement;
      AST_BINARY_EXPRESSION binary_expression;
      AST_ASSIGNMENT assignment;
      struct AST *expression;
      AST_SEQUENCE *sequence;
    };
    Type type_info;
} AST;

#endif