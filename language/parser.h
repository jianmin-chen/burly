#ifndef BURLY_PARSER_H
#define BURLY_PARSER_H

#include "lexer.h"

typedef struct Primitive Primitive;
typedef struct Var Var;
typedef struct Fn Fn;
typedef struct Return Return;
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
  FN,
  RETURN,
  CALL
} NodeType;

typedef enum PrimitiveType
{
  PRIMITIVE_BOOLEAN,
  PRIMITIVE_NUMBER,
  PRIMITIVE_STRING
} PrimitiveType;

struct Node
{
  NodeType type;

  union
  {
    Primitive *primitive;
    UnaryOp *unaryOp;
    BinOp *binOp;
    Var *var;
    Fn *fn;
    Return *ret;
    Call *call;
  };
};

struct Primitive
{
  PrimitiveType type;

  union
  {
    bool b;
    float n;
    char *s;
  };
};

struct Var
{
  Node *value;
  char *name;
};

struct Fn
{
  char *name;
  Node **args;
  Node **argTypes;
  int argCapacity;
  int argCount;
  char *returnType;
  Node **body;
  int bodyCapacity;
  int bodySize;
};

struct Return
{
  Node *value;
};

struct Call
{
  Node *caller;
  Node **args;
  int argCapacity; // Need to store to free later
  int argCount;
};

struct UnaryOp
{
  OpType op;
  Node *right;
};

struct BinOp
{
  Node *left;
  OpType op;
  Node *right;
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

char *nodeToString(Node *node);

void initParser(Parser *parser, Lexer *lexer);
void freeParser(Parser *parser);
void parserPanic(Parser *parser, char *error);
void parse(Parser *parser);

Token *parserPeek(Parser *parser);
Token *parserAdvance(Parser *parser);
bool parserEat(Parser *parser, TokenType type);
Token *parserConsume(Parser *parser, TokenType type);

Node *simple(Parser *parser);
Node *call(Parser *parser);
Node *unary(Parser *parser);
Node *expr(Parser *parser);
Node *stmt(Parser *parser);

void freeNode(Node *n);

#endif