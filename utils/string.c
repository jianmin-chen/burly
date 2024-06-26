#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "string.h"

char *format(char *s, ...)
{
  va_list args;
  va_start(args, s);

  int len = 0;
  char *n = malloc(sizeof(char));
  n[len] = '\0';

  while (*s)
  {
    if (*s == '%')
    {
      s++;
      if (*s == 's')
      {
        char *specifier = va_arg(args, char *);
        len += strlen(specifier);
        realloc(n, sizeof(char) * len);
        strcat(n, specifier);
      }
      else if (*s == 'f')
      {
        double num = va_arg(args, double);
        int nlen = snprintf(NULL, 0, "%f", num);
        char *specifier = malloc(sizeof(char) * (nlen + 1));
        snprintf(specifier, nlen + 1, "%f", num);
        len += nlen;
        realloc(n, sizeof(char) * len);
        strcat(n, specifier);
      }
    }
    else
    {
      realloc(n, ++len);
      n[len - 1] = *s;
    }
    s++;
  }

  va_end(args);

  return n;
}