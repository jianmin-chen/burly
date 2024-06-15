#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "../utils/memory.h"

char *tokenTypeToString(TokenType type)
{
  switch (type)
  {
  case KEYWORD_USE:
    return "KEYWORD_USE";
  case KEYWORD_FROM:
    return "KEYWORD_FROM";
  case KEYWORD_VAR:
    return "KEYWORD_VAR";
  case KEYWORD_FN:
    return "KEYWORD_FN";
  case KEYWORD_CLASS:
    return "KEYWORD_CLASS";
  case KEYWORD_STATIC:
    return "KEYWORD_STATIC";
  case KEYWORD_NEW:
    return "KEYWORD_NEW";
  case KEYWORD_FOR:
    return "KEYWORD_FOR";
  case KEYWORD_WHILE:
    return "KEYWORD_WHILE";
  case KEYWORD_BREAK:
    return "KEYWORD_BREAK";
  case KEYWORD_IF:
    return "KEYWORD_IF";
  case KEYWORD_ELSE:
    return "KEYWORD_ELSE";
  case VALUE:
    return "VALUE";
  case NUMBER:
    return "NUMBER";
  case STRING:
    return "STRING";
  case LEFT_BRACE:
    return "LEFT_BRACE";
  case RIGHT_BRACE:
    return "RIGHT_BRACE";
  case LEFT_PAREN:
    return "LEFT_PAREN";
  case RIGHT_PAREN:
    return "RIGHT_PAREN";
  case LEFT_BRACK:
    return "LEFT_BRACE";
  case RIGHT_BRACK:
    return "RIGHT_BRACK";
  case SINGLE_QUOTE:
    return "SINGLE_QUOTE";
  case DOUBLE_QUOTE:
    return "DOUBLE_QUOTE";
  case SEMICOLON:
    return "SEMICOLON";
  case COLON:
    return "COLON";
  case COMMA:
    return "COMMA";
  case PERIOD:
    return "PERIOD";
  case EQUAL:
    return "EQUAL";
  case EQUIV:
    return "EQUIV";
  case NOT_EQUIV:
    return "NOT_EQUIV";
  case NOT:
    return "NOT";
  case AND:
    return "AND";
  case OR:
    return "OR";
  case LT:
    return "LT";
  case LTE:
    return "LTE";
  case GT:
    return "GT";
  case GTE:
    return "GTE";
  case PLUS:
    return "PLUS";
  case DASH:
    return "DASH";
  case STAR:
    return "STAR";
  case SLASH:
    return "SLASH";
  case BACKSLASH:
    return "BACKSLASH";
  }
}

void initLexer(Lexer *lexer, char *program)
{
  lexer->program = program;
  lexer->current = 0;
  lexer->max = strlen(program);
  lexer->row = 0;
  lexer->col = 0;
  lexer->tokens = NULL;
  lexer->tokenCapacity = 0;
  lexer->tokenCount = 0;
}

void lexerPanic(Lexer *lexer, char *error)
{
  // Format:
  // 2:8 | printf("Hello, world!);
  //            ^ Unexpected end of file
  fprintf(stderr, "%i | ", lexer->row);
  exit(-1);
}

void addToken(Lexer *lexer, TokenType type, char *value)
{
  if (lexer->tokenCapacity < lexer->tokenCount + 1)
  {
    int oldCapacity = lexer->tokenCapacity;
    lexer->tokenCapacity = GROW_CAPACITY(oldCapacity);
    lexer->tokens = GROW_ARRAY(Token, lexer->tokens, oldCapacity, lexer->tokenCapacity);
  }

  Token token;
  token.type = type;
  token.value = value;
  lexer->tokens[lexer->tokenCount++] = token;
}

char peek(Lexer *lexer)
{
  if (lexer->current >= lexer->max)
    return '\0';
  return lexer->program[lexer->current];
}

char advance(Lexer *lexer)
{
  if (lexer->current >= lexer->max)
    return '\0';
  return lexer->program[lexer->current++];
}

bool eat(Lexer *lexer, char chr)
{
  if (peek(lexer) == chr)
  {
    advance(lexer);
    return true;
  }
  return false;
}

void scanToken(Lexer *lexer)
{
  char chr = advance(lexer);
  lexer->col++;
  switch (chr)
  {
  case '{':
    return addToken(lexer, LEFT_BRACE, "{");
  case '}':
    return addToken(lexer, RIGHT_BRACE, "}");
  case '(':
    return addToken(lexer, LEFT_PAREN, "(");
  case ')':
    return addToken(lexer, RIGHT_PAREN, ")");
  case '[':
    return addToken(lexer, LEFT_BRACK, "[");
  case ']':
    return addToken(lexer, RIGHT_BRACK, "]");
  case '\'':
  {
    char *s = malloc(sizeof(char));
    while (peek(lexer) != '\'')
    {
      if (peek(lexer) == '\0')
        lexerPanic(lexer, "Unexpected end of file");
      int size = strlen(s);
      realloc(s, sizeof(char) * (size + 2));
      s[size] = advance(lexer);
    }
    advance(lexer);
    return addToken(lexer, SINGLE_QUOTE, s);
  }
  case '"':
  {
    char *s = malloc(sizeof(char));
    while (peek(lexer) != '"')
    {
      if (peek(lexer) == '\0')
        lexerPanic(lexer, "Unexpected end of file");
      int size = strlen(s);
      realloc(s, sizeof(char) * (size + 2));
      s[size] = advance(lexer);
    }
    advance(lexer);
    return addToken(lexer, DOUBLE_QUOTE, s);
  }
  case ';':
    return addToken(lexer, SEMICOLON, ";");
  case ':':
    return addToken(lexer, COLON, ":");
  case ',':
    return addToken(lexer, COMMA, ",");
  case '.':
    return addToken(lexer, PERIOD, ".");
  case '=':
    if (eat(lexer, '='))
      return addToken(lexer, EQUIV, "==");
    return addToken(lexer, EQUAL, "=");
  case '!':
    if (eat(lexer, '='))
      return addToken(lexer, NOT_EQUIV, "!=");
    return addToken(lexer, NOT, "!");
  case '&':
    if (eat(lexer, '&'))
      return addToken(lexer, AND, "&&");
  case '|':
    if (eat(lexer, '|'))
      return addToken(lexer, OR, "||");
  case '<':
    if (eat(lexer, '='))
      return addToken(lexer, LTE, "<=");
    return addToken(lexer, LT, "<");
  case '>':
    if (eat(lexer, '='))
      return addToken(lexer, GTE, ">=");
    return addToken(lexer, GT, ">");
  case '+':
    return addToken(lexer, PLUS, "+");
  case '-':
    if (eat(lexer, '='))
    {
      while (advance(lexer) != '\n' || peek(lexer) != '\0')
        ;
      return;
    }
    return addToken(lexer, DASH, "-");
  case '*':
    return addToken(lexer, STAR, "*");
  case '/':
    return addToken(lexer, SLASH, "/");
  case '\\':
    return addToken(lexer, BACKSLASH, "\\");
  case '\n':
    lexer->row++;
  case '\t':
  case '\r':
  case ' ':
    return;
  default:
    if (isdigit(chr))
    {
      char *s = malloc(sizeof(char) * 2);
      *s = chr;
      bool period = false;
      while (isdigit(peek(lexer)) || (!period && peek(lexer) == '.'))
      {
        char next = advance(lexer);
        int size = strlen(s);
        realloc(s, sizeof(char) * (size + 2));
        s[size] = next;
        if (next == '.')
          period = true;
      }
      return addToken(lexer, NUMBER, s);
    }
    else if (isalpha(chr))
    {
      char *s = malloc(sizeof(char) * 2);
      *s = chr;
      while (isalpha(peek(lexer)) || isdigit(peek(lexer)))
      {
        int size = strlen(s);
        realloc(s, sizeof(char) * (size + 2));
        s[size] = advance(lexer);
      }
      if (strcmp(s, "use") == 0)
        return addToken(lexer, KEYWORD_USE, s);
      else if (strcmp(s, "from") == 0)
        return addToken(lexer, KEYWORD_FROM, s);
      else if (strcmp(s, "var") == 0)
        return addToken(lexer, KEYWORD_VAR, s);
      else if (strcmp(s, "fn") == 0)
        return addToken(lexer, KEYWORD_FN, s);
      else if (strcmp(s, "class") == 0)
        return addToken(lexer, KEYWORD_CLASS, s);
      else if (strcmp(s, "static") == 0)
        return addToken(lexer, KEYWORD_STATIC, s);
      else if (strcmp(s, "new") == 0)
        return addToken(lexer, KEYWORD_NEW, s);
      else if (strcmp(s, "for") == 0)
        return addToken(lexer, KEYWORD_FOR, s);
      else if (strcmp(s, "while") == 0)
        return addToken(lexer, KEYWORD_WHILE, s);
      else if (strcmp(s, "break") == 0)
        return addToken(lexer, KEYWORD_BREAK, s);
      else if (strcmp(s, "if") == 0)
        return addToken(lexer, KEYWORD_IF, s);
      else if (strcmp(s, "else") == 0)
        return addToken(lexer, KEYWORD_ELSE, s);
      return addToken(lexer, VALUE, s);
    }
    lexerPanic(lexer, "Unexpected symbol");
  }
}

void scanTokens(Lexer *lexer)
{
  while (peek(lexer) != '\0')
  {
    scanToken(lexer);
  }
}