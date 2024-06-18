#include <stdlib.h>
#include "parser.h"
#include "../utils/memory.h"
#include <stdio.h>

void initParser(Parser *parser, Lexer *lexer)
{
  parser->nodes = NULL;
  parser->nodeCapacity = 0;
  parser->nodeCount = 0;
  parser->current = 0;
  parser->tokens = lexer->tokens;
  parser->tokenCount = lexer->tokenCount;
}

void parserPanic(Parser *parser, char *error)
{
  fprintf(stderr, "%s", error);
  exit(-2);
}

void addNode(Parser *parser, Node *node)
{
  if (parser->nodeCapacity < parser->nodeCount + 1)
  {
    int oldCapacity = parser->nodeCapacity;
    parser->nodeCapacity = GROW_CAPACITY(oldCapacity);
    parser->nodes = GROW_ARRAY(Node, parser->nodes, oldCapacity, parser->nodeCapacity);
  }

  parser->nodes[parser->nodeCount++] = *node;
}

Node *newNode(NodeType type)
{
  Node *node = malloc(sizeof(Node));
  node->type = type;
  return node;
}

Token parserPeek(Parser *parser)
{
  if (parser->current >= parser->tokenCount)
    return parser->tokens[parser->tokenCount - 1];
  return parser->tokens[parser->current];
}

Token parserAdvance(Parser *parser)
{
  if (parser->current >= parser->tokenCount)
    return parser->tokens[parser->tokenCount - 1];
  return parser->tokens[parser->current++];
}

bool parserEat(Parser *parser, TokenType type)
{
  if (parserPeek(parser).type == type)
  {
    parserAdvance(parser);
    return true;
  }
  return false;
}

Token parserConsume(Parser *parser, TokenType type)
{
  if (parserPeek(parser).type == type)
    return parserAdvance(parser);
  parserPanic(parser, "Expected a token but got something else");
}

bool isOp(Token token)
{
  switch (token.type)
  {
  case EQUIV:
  case NOT_EQUIV:
  case NOT:
  case AND:
  case OR:
  case LT:
  case LTE:
  case GT:
  case GTE:
  case PLUS:
  case DASH:
  case STAR:
  case SLASH:
    return true;
  default:
    return false;
  }
}

OpType processOp(Parser *parser, Token token)
{
  switch (token.type)
  {
  case EQUIV:
    return OP_EQUIV;
  case NOT_EQUIV:
    return NOT_EQUIV;
  case NOT:
    return OP_NOT;
  case AND:
    return OP_AND;
  case OR:
    return OP_OR;
  case LT:
    return OP_LT;
  case LTE:
    return OP_LTE;
  case GT:
    return OP_GT;
  case GTE:
    return OP_GTE;
  case PLUS:
    return OP_ADD;
  case DASH:
    return OP_SUB;
  case STAR:
    return OP_MUL;
  case SLASH:
    return OP_DIV;
  default:
    parserPanic(parser, "Expected a operator");
  }
}

Node *simple(Parser *parser)
{
  Token token = parserAdvance(parser);
  switch (token.type)
  {
  case NUMBER:
  {
    Primitive *num = malloc(sizeof(Primitive));
    num->i = (int)token.value;

    Node *simple = newNode(PRIMITIVE);
    simple->primitive = num;
    printf("%i\n", token.value);
    return simple;
  }
  default:
    parserPanic(parser, "Not appropriate simple");
  }
}

Node *call(Parser *parser)
{
  Node *caller = simple(parser);

  if (parserEat(parser, LEFT_PAREN) || parserEat(parser, LEFT_BRACK))
  {
    int argCount = 0;
    // TODO
  }

  return caller;
}

Node *unary(Parser *parser)
{
  if (isOp(parserPeek(parser)))
  {
    UnaryOp *unaryOp = malloc(sizeof(UnaryOp));
    unaryOp->op = processOp(parser, parserAdvance(parser));
    unaryOp->right = *unary(parser);

    Node *unary = newNode(UNARY_OP);
    unary->unaryOp = unaryOp;
    return unary;
  }

  return call(parser);
}

Node *expr(Parser *parser)
{
  Node *left = unary(parser);

  if (isOp(parserPeek(parser)))
  {
    OpType op = processOp(parser, parserAdvance(parser));
    Node *right = unary(parser);

    BinOp *binOp = malloc(sizeof(BinOp));
    binOp->left = *left;
    binOp->op = op;
    binOp->right = *right;

    Node *expr = newNode(BIN_OP);
    expr->binOp = binOp;
    return expr;
  }

  return left;
}

Node *assignStmt(Parser *parser)
{
  parserConsume(parser, KEYWORD_VAR);
  char *name = parserConsume(parser, VALUE).value;
  parserConsume(parser, EQUAL);
  Node *value = expr(parser);
  printf("%i", value->primitive->i);
  printf("done");
  exit(1);
  parserConsume(parser, SEMICOLON);
}

Node *stmt(Parser *parser)
{
  switch (parserPeek(parser).type)
  {
  case KEYWORD_VAR:
    return assignStmt(parser);
  default:
  }
}

void parse(Parser *parser)
{
  while (parserPeek(parser).type != END_OF_FILE)
  {
    addNode(parser, stmt(parser));
    break;
  }
}
