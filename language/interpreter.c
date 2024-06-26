#include "interpreter.h"
#include <stdio.h>

#define RET_TYPE float

void output(Node **args)
{
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
    // void *caller = getMap(scope, node->call->caller->var->name);
    // void (*f)();
    // f = caller;
    // f();
    // Fn *caller = getMap(scope, node->call->caller->var->name);
    // float (*f)(float a, float b);
    // f(caller->args[0]->primitive->n, caller->args[1]->primitive->n);
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
  case FN:
  {
    setMap(scope, node->fn->name, node->fn);
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