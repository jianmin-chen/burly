#include <stdio.h>
#include <stdlib.h>
#include "language/lexer.h"
#include "language/parser.h"
#include "language/interpreter.h"

static char *readFile(const char *path)
{
  FILE *file = fopen(path, "rb");
  if (file == NULL)
  {
    fprintf(stderr, "Could not open file \"%s\".\n", path);
    exit(74);
  }

  fseek(file, 0L, SEEK_END);
  size_t fileSize = ftell(file);
  rewind(file);

  char *buffer = (char *)malloc(fileSize + 1);
  if (buffer == NULL)
  {
    fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
    exit(74);
  }
  size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
  buffer[bytesRead] = '\0';
  if (bytesRead < fileSize)
  {
    fprintf(stderr, "Could not read file \"%s\".\n", path);
    exit(74);
  }

  fclose(file);
  return buffer;
}

static void repl()
{
}

int main(int argc, char *argv[])
{
  int debug = 1;
  for (int i = 1; i < argc; i++)
  {
    if (argv[i] == "--debug")
    {
      debug = i;
      break;
    }
  }

  if (argc == 2)
  {
    char *program = readFile(argv[1]);
    Lexer lexer;
    initLexer(&lexer, program);
    scanTokens(&lexer);

    if (debug)
    {
      FILE *fptr = fopen("lexer.json", "w");
      fprintf(fptr, "[ ");
      for (int i = 0; i < lexer.tokenCount; i++)
      {
        Token tok = lexer.tokens[i];
        fprintf(fptr, "{ \"type\": \"%s\", \"value\": \"%s\", \"row\": %i, \"col\": %i }", tokenTypeToString(tok.type), tok.value, tok.row, tok.col);
        if (i != lexer.tokenCount - 1)
          fprintf(fptr, ", ");
      }
      fprintf(fptr, " ]");
      fclose(fptr);
    }

    Parser parser;
    initParser(&parser, &lexer);
    parse(&parser);

    if (debug)
    {
      FILE *fptr = fopen("parser.json", "w");
      fprintf(fptr, "[ ");
      for (int i = 0; i < parser.nodeCount; i++)
      {
        Node node = parser.nodes[i];
        fprintf(fptr, "%s", nodeToString(&node));
        if (i != parser.nodeCount - 1)
          fprintf(fptr, ", ");
      }
      fprintf(fptr, " ]");
      fclose(fptr);
    }

    Map *scope = defaultScope();
    interpret(&parser, scope);

    for (int i = 0; i < scope->length; i++)
    {
      char *key = scope->keys[i];
      printf("%s: %s\n", key, nodeToString(getMap(scope, key)));
    }

    free(scope);

    free(program);
  }
  else
  {
    repl();
  }

  return 0;
}