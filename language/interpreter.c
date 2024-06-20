#include "interpreter.h"
#include <stdio.h>

void output(Node *arg)
{
  printf("%s\n", nodeToString(arg));
}

Map *defaultScope()
{
  // Testing purposes right now.
  Map *scope = malloc(sizeof(Map));
  initMap(scope);
  setMap(scope, "output", &output);
  return scope;
}

void evaluate(Node *node, Map *scope)
{
  switch (node->type)
  {
  case CALL:
  {
    void *caller = getMap(scope, node->call->caller->var->name);
    void (*f)(Node *arg);
    f = caller;
    f(node->call->args[0]);
  }
  }
}

void execute(Node *node, Map *scope)
{
  switch (node->type)
  {
  case VAR:
  {
    setMap(scope, node->var->name, node->var->value);
    break;
  }
  default:
    evaluate(node, scope);
  }
}

void interpret(Parser *parser, Map *scope)
{
  for (int i = 0; i < parser->nodeCount; i++)
  {
    execute(&parser->nodes[i], scope);
  }
}