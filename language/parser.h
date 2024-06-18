#ifndef BURLY_PARSER_H
#define BURLY_PARSER_H

#include "lexer.h"

typedef struct Primitive Primitive;
typedef struct Var Var;
typedef struct Call Call;
typedef struct UnaryOp UnaryOp;
typedef struct BinOp BinOp;
typedef struct Node Node;
typedef enum OpType
{
  OP_EQUIV,
  OP_NOT_EQUIV,
  OP_NOT,
  OP_AND,
  OP_OR,
  OP_LT,
  OP_LTE,
  OP_GT,
  OP_GTE,
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_DIV
} OpType;

typedef enum NodeType
{
  PRIMITIVE,
  UNARY_OP,
  BIN_OP,
  VAR,
  CALL
} NodeType;

struct Node
{
  NodeType type;

  union
  {
    Primitive *primitive;
    UnaryOp *unaryOp;
    BinOp *binOp;
    Var *var;
    Call *call;
  };
};

struct Primitive
{
  union
  {
    int i;
    float f;
    char *s;
  };
};

struct Var
{
  Node value;
  char *name;
};

struct Call
{
  char *caller;
  Node *args;
  int argCount;
};

struct UnaryOp
{
  OpType op;
  Node right;
};

struct BinOp
{
  Node left;
  OpType op;
  Node right;
};

typedef struct Parser
{
  Node *nodes;
  int nodeCapacity;
  int nodeCount;
  int current;
  Token *tokens;
  int tokenCount;
} Parser;

void initParser(Parser *parser, Lexer *lexer);
void parserPanic(Parser *parser, char *error);
void parse(Parser *parser);

Token parserPeek(Parser *parser);
Token parserAdvance(Parser *parser);
bool parserEat(Parser *parser, TokenType type);
Token parserConsume(Parser *parser, TokenType type);

#endif