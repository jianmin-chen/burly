#include <stdlib.h>
#include <stdio.h>
#include "parser.h"
#include "../utils/memory.h"
#include "../utils/string.h"

char *primitiveToString(Primitive *primitive)
{
  switch (primitive->type)
  {
  case PRIMITIVE_NUMBER:
    return format("%f", primitive->n);
  case PRIMITIVE_STRING:
    return primitive->s;
  }
}

char *nodeToString(Node *node)
{
  switch (node->type)
  {
  case PRIMITIVE:
  {
    char *s = format("{ \"type\": \"PRIMITIVE\", \"value\": \"%s\" }", primitiveToString(node->primitive));
    return s;
  }
  case UNARY_OP:
  {
    char *s = "{ \"type\": \"UNARY_OP\"";
    return s;
  }
  case BIN_OP:
  {
    char *s = "{ \"type\": \"BIN_OP\"";
    return s;
  }
  case VAR:
  {
    char *s = "{ \"type\": \"VAR\"";
    return s;
  }
  case FN:
  {
    char *s = "{ \"type\": \"FN\"";
    return s;
  }
  case CALL:
  {
    char *s = "{ \"type\": \"CALL\", ";
    return s;
  }
  default:
    return "undefined";
  }
}

void initParser(Parser *parser, Lexer *lexer)
{
  parser->nodes = NULL;
  parser->nodeCapacity = 0;
  parser->nodeCount = 0;
  parser->current = 0;
  parser->tokens = lexer->tokens;
  parser->tokenCount = lexer->tokenCount;
}

void freeParser(Parser *parser)
{
  for (int i = 0; i < parser->nodeCount; i++)
  {
    freeNode(&parser->nodes[i]);
  }
}

void parserPanic(Parser *parser, char *error)
{
  // Format:
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

void freeNode(Node *n) {}

Token *parserPeek(Parser *parser)
{
  if (parser->current >= parser->tokenCount)
    return &parser->tokens[parser->tokenCount - 1];
  return &parser->tokens[parser->current];
}

Token *parserAdvance(Parser *parser)
{
  if (parser->current >= parser->tokenCount)
    return &parser->tokens[parser->tokenCount - 1];
  return &parser->tokens[parser->current++];
}

bool parserEat(Parser *parser, TokenType type)
{
  if (parserPeek(parser)->type == type)
  {
    parserAdvance(parser);
    return true;
  }
  return false;
}

Token *parserConsume(Parser *parser, TokenType type)
{
  if (parserPeek(parser)->type == type)
    return parserAdvance(parser);
  printf("%s", tokenTypeToString(type));
  parserPanic(parser, "Expected a token but got something else");
}

bool isOp(TokenType type)
{
  switch (type)
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

OpType processOp(Parser *parser, TokenType type)
{
  switch (type)
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

Node *id(Parser *parser, Token *token)
{
  if (token->type != VALUE)
    parserPanic(parser, "Invalid syntax");

  Var *ref = malloc(sizeof(Var));
  ref->name = token->value;

  Node *id = newNode(VAR);
  id->var = ref;
  return id;
}

Node *simple(Parser *parser)
{
  Token *token = parserAdvance(parser);
  switch (token->type)
  {
  case NUMBER:
  {
    Primitive *num = malloc(sizeof(Primitive));
    num->type = PRIMITIVE_NUMBER;
    num->n = atof(token->value);

    Node *simple = newNode(PRIMITIVE);
    simple->primitive = num;
    return simple;
  }
  case VALUE:
    return id(parser, token);
  default:
    printf("type: %s\n", tokenTypeToString(token->type));
    parserPanic(parser, "Not appropriate simple");
  }
}

Node *call(Parser *parser)
{
  Node *caller = simple(parser);

  while (parserPeek(parser)->type == LEFT_PAREN || parserPeek(parser)->type == LEFT_BRACK)
  {
    TokenType closing = parserAdvance(parser)->type == LEFT_PAREN ? RIGHT_PAREN : RIGHT_BRACK;
    int argCapacity = GROW_CAPACITY(0);
    int argCount = 0;
    Node **args = NULL;
    args = GROW_ARRAY(Node *, args, 0, argCapacity);
    args[argCount++] = expr(parser);
    while (parserEat(parser, COMMA))
    {
      Node *arg = expr(parser);
      if (argCapacity < argCount + 1)
      {
        int oldCapacity = argCapacity;
        argCapacity = GROW_CAPACITY(oldCapacity);
        args = GROW_ARRAY(Node *, args, oldCapacity, argCapacity);
      }
      args[argCount++] = arg;
    }
    parserConsume(parser, closing);

    Call *call = malloc(sizeof(Call));
    call->argCount = argCount;
    call->argCapacity = argCapacity;
    call->args = args;
    call->caller = caller;

    Node *swap = newNode(CALL);
    swap->call = call;
    caller = swap;
  }

  return caller;
}

Node *unary(Parser *parser)
{
  if (isOp(parserPeek(parser)->type))
  {
    UnaryOp *unaryOp = malloc(sizeof(UnaryOp));
    unaryOp->op = processOp(parser, parserAdvance(parser)->type);
    unaryOp->right = unary(parser);

    Node *unary = newNode(UNARY_OP);
    unary->unaryOp = unaryOp;
    return unary;
  }

  return call(parser);
}

Node *expr(Parser *parser)
{
  Node *left = unary(parser);

  if (isOp(parserPeek(parser)->type))
  {
    OpType op = processOp(parser, parserAdvance(parser)->type);
    Node *right = unary(parser);

    BinOp *binOp = malloc(sizeof(BinOp));
    binOp->left = left;
    binOp->op = op;
    binOp->right = right;

    Node *expr = newNode(BIN_OP);
    expr->binOp = binOp;
    return expr;
  }

  return left;
}

Node *assignStmt(Parser *parser)
{
  parserConsume(parser, KEYWORD_VAR);
  char *name = parserConsume(parser, VALUE)->value;
  parserConsume(parser, EQUAL);
  Node *value = expr(parser);

  Var *var = malloc(sizeof(Var));
  var->name = name;
  var->value = value;

  Node *assign = newNode(VAR);
  assign->var = var;
  return assign;
}

Node *fnStmt(Parser *parser)
{
  Fn *fn = malloc(sizeof(Fn));

  parserConsume(parser, KEYWORD_FN);
  fn->name = parserConsume(parser, VALUE)->value;

  parserConsume(parser, LEFT_PAREN);
  int argCapacity = GROW_CAPACITY(0);
  int argCount = 0;
  Node **args = NULL;
  Node **argTypes = NULL;
  args = GROW_ARRAY(Node *, args, 0, argCapacity);
  argTypes = GROW_ARRAY(Node *, argTypes, 0, argCapacity);
  while (!parserEat(parser, RIGHT_PAREN))
  {
    // Eat arguments, grab types
    Node *arg = id(parser, parserAdvance(parser));
    if (argCapacity < argCount + 1)
    {
      int oldCapacity = argCapacity;
      argCapacity = GROW_CAPACITY(oldCapacity);
      args = GROW_ARRAY(Node *, args, oldCapacity, argCapacity);
      argTypes = GROW_ARRAY(Node *, args, oldCapacity, argCapacity);
    }
    args[argCount] = arg;

    parserConsume(parser, COLON);
    argTypes[argCount++] = id(parser, parserAdvance(parser));

    if (parserPeek(parser)->type != RIGHT_PAREN)
      parserConsume(parser, COMMA);
  }
  fn->argCapacity = argCapacity;
  fn->argCount = argCount;
  fn->args = args;

  if (parserEat(parser, DASH))
  {
    // There is a provided return type, no need to deduce
    parserConsume(parser, GT);
    fn->returnType = id(parser, parserAdvance(parser));
  }

  parserConsume(parser, LEFT_BRACE);
  int bodyCapacity = GROW_CAPACITY(0);
  int bodySize = 0;
  Node **body = NULL;
  body = GROW_ARRAY(Node *, body, 0, bodyCapacity);
  while (!parserEat(parser, RIGHT_BRACE))
  {
    // Eat up body, statement by statement
    Node *line = stmt(parser);
    if (bodyCapacity < bodySize + 1)
    {
      int oldCapacity = bodyCapacity;
      bodyCapacity = GROW_CAPACITY(oldCapacity);
      body = GROW_ARRAY(Node *, body, oldCapacity, bodyCapacity);
    }
    body[bodySize++] = line;
  }
  fn->bodyCapacity = bodyCapacity;
  fn->bodySize = bodySize;
  fn->body = body;

  Node *func = newNode(FN);
  func->fn = fn;
  return func;
}

Node *returnStmt(Parser *parser)
{
  parserConsume(parser, KEYWORD_RETURN);
  Return *ret = malloc(sizeof(Return));
  ret->value = expr(parser);
  Node *stmt = newNode(RETURN);
  stmt->ret = ret;
  return ret;
}

Node *stmt(Parser *parser)
{
  Node *stmt = NULL;
  switch (parserPeek(parser)->type)
  {
  case KEYWORD_VAR:
    stmt = assignStmt(parser);
    break;
  case KEYWORD_FN:
    stmt = fnStmt(parser);
    break;
  case KEYWORD_RETURN:
    stmt = returnStmt(parser);
    break;
  default:
    stmt = expr(parser);
    break;
  }
  parserConsume(parser, SEMICOLON);
  return stmt;
}

void parse(Parser *parser)
{
  while (parserPeek(parser)->type != END_OF_FILE)
  {
    addNode(parser, stmt(parser));
  }
}
