#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void *reallocate(void *pointer, size_t oldSize, size_t newSize)
{
  if (newSize == 0)
  {
    free(pointer);
    return NULL;
  }

  void *result = realloc(pointer, newSize);
  if (result == NULL)
    exit(1);
  return result;
}

int main()
{
  char t = 't';
  char u = 'u';
  char *s = malloc(sizeof(char));
  *s = u;
  // printf("%s", s);
  int size = strlen(s);
  realloc(s, sizeof(char) + 1);
  s[size] = t;
  printf("%c\n", s[2]);
}