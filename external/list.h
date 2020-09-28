#ifndef LIST_H
#define LIST_H
#include <stdio.h>
#include <stdlib.h>

typedef struct list_node_t list_node_t;
struct list_node_t {
	void *data;
	list_node_t *next;
};

typedef struct list_base_t {
	int size;
	list_node_t *root;
} list_base_t;

#define list_t(T) \
	struct { list_base_t list; T *ref; T tmp; }

#define list_init(list) \
	memset((list), 0, sizeof(*(list)));

#define list_insert(list, val) \
	((list)->tmp = val, \
		list_insert_(&(list)->base, &(list)->tmp, sizeof((list)->tmp)))

void list_insert_(list_base_t *base, void *data, int dsize);
void list_remove_(list_base_t *base, int i);
void *list_get_(list_base_t *base, int i);
void list_clear_(list_base_t *base);

typedef list_t(int) list_int_t;
typedef list_t(void*) list_void_t;
typedef list_t(char*) list_char_t;

#endif

#if defined(LIST_IMPLEMENTATION)

static list_node_t *list_newnode(void *data, int dsize) {
	list_node_t *node;
	node = malloc(sizeof(*node) + dsize);
	// node->data = ((char*) node) + dsize;
	node->value = ((char*) node);
	node->next = NULL;
	memcpy(node->value, data, dsize);

	return node;
}

void list_insert_(list_base_t *base, void *data, int dsize) {
	list_node_t *node = base->root;
	if (base->size <= 0) {
		base->root = list_newnode(data, dsize);
		return;
	}

	while (node->next) {
		node = node->next;
	}

	list_node_t *new_node = list_newnode(data, dsize);
	node->next = new_node;
}

#endif