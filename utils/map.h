#ifndef BURLY_MAP_H
#define BURLY_MAP_H

#include <stdlib.h>

typedef struct MapEntry
{
  const char *key;
  void *value;
} MapEntry;

typedef struct Map
{
  MapEntry *entries;
  char **keys;
  int capacity;
  int length;
} Map;

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

uint64_t hashKey(const char *key);

void initMap(Map *map);
void destroyMap(Map *map);
const char *setMap(Map *map, const char *key, void *value);
void *getMap(Map *map, const char *key);

#endif