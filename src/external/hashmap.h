#ifndef HASHMAP_H
#define HASHMAP_H

#include <string.h>

typedef struct hashmap_item hashmap_item;

struct hashmap_item {
  char *key;
  int value;
  hashmap_item *next;
};

typedef struct {
  int size;
  hashmap_item **map;
} hashmap;

hashmap hashmap_create(int size);
hashmap_item* hashmap_get(hashmap hash, const char *key);
void hashmap_set(hashmap *hash, const char *key, int value);
void hashmap_remove(hashmap *hash, const char *key);

#endif

#if defined(HASHMAP_IMPLEMENTATION)


// Você pode criar seu próprio algoritmo de hash
static unsigned int hashCode(const char *key) {
  unsigned int hash = 0;
  char c = *key;
  while (c) {
    hash = (hash << 5) + c;
    c = *key++;
  }
  return hash;
}

static hashmap_item *create_hash_item(const char *key, int value) {
  hashmap_item *item = malloc(sizeof(*item));
  item->key = malloc(strlen(key) + 1);
  strcpy(item->key, key);
  item->value = value;
  item->next = NULL;
  return item;
}

hashmap hashmap_create(int size) {
  hashmap map = {0};
  if (size <= 0) {
    fprintf(stderr, "size need to be greater than 0\n");
    exit(0);
  }
  map.size = size;
  map.map = calloc(sizeof(hashmap_item*), size);

  return map;
}

hashmap_item *hashmap_get(hashmap hash, const char *key) {
  int hashIndex = hashCode(key) % hash.size;

  hashmap_item *item = hash.map[hashIndex];
  while(item != NULL) {
    if (!strcmp(item->key, key)) return item;

    item = item->next;
  }

  return NULL;
}

void hashmap_set(hashmap *hash, const char *key, int value) {
  int hashIndex = hashCode(key) % hash->size;
//   TRACELOG("%s %d %d", key, value, hashIndex);

  hashmap_item *temp = hash->map[hashIndex];

  if (temp == NULL) {
    hashmap_item *new = create_hash_item(key, value);
    hash->map[hashIndex] = new;
    return;
  }
  while(temp->next != NULL) {
    temp = temp->next;
  }

  hashmap_item *item = create_hash_item(key, value);

  temp->next = item;
}

void hashmap_remove(hashmap *hash, const char *key) {
  int hashIndex = hashCode(key) % hash->size;

  hashmap_item *item = hash->map[hashIndex];
  if (!item) return;
  if (!strcmp(item->key, key)) {
    hash->map[hashIndex] = item->next;
    free(item);
    return;
  }

  while (item->next) {
    if (!strcmp(item->next->key, key)) {
      break;
    }
    item = item->next;
  }
  hashmap_item *delItem = item->next;
  item->next = delItem->next;
  free(delItem);
}

#endif
