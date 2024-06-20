#ifndef BURLY_INTERPRETER_H
#define BURLY_INTERPRETER_H

#include "parser.h"
#include "../utils/map.h"

Map *defaultScope(void);
void interpret(Parser *parser, Map *scope);

#endif