#ifndef BURLY_LEXER_H
#define BURLY_LEXER_H

#include <stdbool.h>

typedef enum TokenType
{
  KEYWORD_USE,
  KEYWORD_FROM,
  KEYWORD_VAR,
  KEYWORD_FN,
  KEYWORD_RETURN,
  KEYWORD_CLASS,
  KEYWORD_STATIC,
  KEYWORD_NEW,
  KEYWORD_FOR,
  KEYWORD_WHILE,
  KEYWORD_BREAK,
  KEYWORD_IF,
  KEYWORD_ELSE,
  VALUE,
  NUMBER,
  STRING,
  LEFT_BRACE,
  RIGHT_BRACE,
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_BRACK,
  RIGHT_BRACK,
  SEMICOLON,
  COLON,
  COMMA,
  PERIOD,
  EQUAL,
  EQUIV,
  NOT_EQUIV,
  NOT,
  AND,
  OR,
  LT,
  LTE,
  GT,
  GTE,
  PLUS,
  DASH,
  STAR,
  SLASH,
  BACKSLASH,
  END_OF_FILE
} TokenType;

char *tokenTypeToString(TokenType type);

typedef struct Token
{
  TokenType type;
  char *value;
  int row;
  int col;
} Token;

typedef struct Lexer
{
  char *program;
  int current;
  int max;
  int row;
  int col;
  Token *tokens;
  int tokenCapacity;
  int tokenCount;
} Lexer;

void initLexer(Lexer *lexer, char *program);
void lexerPanic(Lexer *lexer, char *error);
void addToken(Lexer *lexer, TokenType type, char *value);
void scanTokens(Lexer *lexer);
void scanToken(Lexer *lexer);

char lexerPeek(Lexer *lexer);
char lexerAdvance(Lexer *lexer);
bool lexerEat(Lexer *lexer, char chr);

#endif