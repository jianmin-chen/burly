#include <string.h>
#include <stdio.h>
#include "map.h"
#include "memory.h"

uint64_t hashKey(const char *key)
{
  uint64_t hash = FNV_OFFSET;
  for (const char *p = key; *p; p++)
  {
    hash ^= (uint64_t)(unsigned char)(*p);
    hash *= FNV_PRIME;
  }
  return hash;
}

void initMap(Map *map)
{
  map->entries = NULL;
  map->capacity = 0;
  map->length = 0;
}

void destroyMap(Map *map)
{
  free(map);
}

const char *setMap(Map *map, const char *key, void *value)
{
  if (map->capacity < map->length + 1)
  {
    int oldCapacity = map->capacity;
    map->capacity = GROW_CAPACITY(oldCapacity);
    map->entries = GROW_ARRAY(void *, map->entries, oldCapacity, map->capacity);
    map->keys = GROW_ARRAY(char *, map->keys, oldCapacity, map->capacity);
  }

  MapEntry entry;
  entry.key = key;
  entry.value = value;

  uint64_t hash = hashKey(key);
  size_t index = (size_t)(hash & (uint64_t)(map->capacity - 1));
  while (map->entries[index].key != NULL)
  {
    // Loop until we find an entry to update
    if (strcmp(key, map->entries[index].key) == 0)
    {
      map->entries[index].value = value;
      return map->entries[index].key;
    }
    index++;
    if (index >= map->capacity)
      index = 0;
  }

  // Didn't find key, insert it
  map->entries[index].key = (char *)key;
  map->entries[index].value = value;
  map->keys[map->length++] = (char *)key;
  return key;
}

void *getMap(Map *map, const char *key)
{
  uint64_t hash = hashKey(key);
  size_t index = (size_t)(hash & (uint64_t)(map->capacity - 1));

  while (map->entries[index].key != NULL)
  {
    if (strcmp(key, map->entries[index].key) == 0)
    {
      return map->entries[index].value;
    }
    index++;
    if (index >= map->capacity)
      index = 0;
  }

  return NULL;
}