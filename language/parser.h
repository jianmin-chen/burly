#ifndef BURLY_PARSER_H
#define BURLY_PARSER_H

#include "lexer.h"

typedef struct Node
{

} Node;

typedef struct Parser
{
  Node *nodes;
  int nodeCapacity;
  int nodeCount;
  int current;
  Token *tokens;
  int tokenCount;
} Parser;

#endif