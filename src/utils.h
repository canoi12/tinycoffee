#ifndef TICO_UTILS_H
#define TICO_UTILS_H

/******************
 * Stack Array
 ******************/

#define stack_arr_t(T, sz) \
	struct { T data[sz]; int top, size; }

#define stack_arr_init(s, n) \
	(memset((s), 0, sizeof(*(s))), \
		(s)->size = n)

#define stack_arr_push(s, v) \
	if ((s)->top < (s)->size) (s)->data[(s)->top++] = (v)

#define stack_arr_top(s) \
	((s)->top <= 0 ? (s)->data[0] : (s)->data[(s)->top - 1])

#define stack_arr_pop(s) \
	((s)->top <= 0 ? (s)->data[0] : (s)->data[--(s)->top])

#define stack_arr_set(s, d) \
	((s)->top <= 0 ? memcpy(&(s)->data[0], &(d), sizeof((d))) : memcpy(&(s)->data[(s)->top - 1], &(d), sizeof((d))))

#define stack_arr_clear(s) \
	memset((s), 0, sizeof(*(s)))

#define stack_arr_destroy(s) \
	free((s)->data)

/*****************
 * Linked List
 *****************/


typedef struct list_node_t list_node_t;

struct list_node_t {
	void *data;
	list_node_t *next;
};

typedef struct list_base_t {
	int size;
	list_node_t *root;
	list_node_t *last;
} list_base_t;

typedef struct list_iter_t {
	list_node_t *node;
	int index;
	void *data;
} list_iter_t;

#define list_t(T) \
	struct { list_base_t base; T *ref; T tmp; }

#define list_init(l) \
	memset((l), 0, sizeof(*(l)))

#define list_push(l, val) \
	((l)->tmp = (val), \
	list_push_(&(l)->base, &(l)->tmp, sizeof((l)->tmp)))

#define list_pop(l, d) \
	list_pop_(&(l)->base)

#define list_last(l) \
	list_last_(&(l)->base)

#define list_remove(l, i) \
	list_remove_(&(l)->base, (i))

#define list_get(l, i) \
	list_get_(&(l)->base)

#define list_set(l, i, v) \
	((l)->tmp = v, \
		list_set_(&(l)->base, (i), (l)->tmp, sizeof((l)->tmp)))

#define list_clear(l) \
	list_clear_(&(l)->base)

#define list_iter(l) \
	list_iter_(&(l)->base)

#define list_next(iter) \
	list_next_(iter)

void list_push_(list_base_t *base, void *val, int dsize);
void* list_pop_(list_base_t *base);
void* list_last_(list_base_t *base);
void list_remove_(list_base_t *base, int i);
void* list_get_(list_base_t *base, int i);
void list_set_(list_base_t *base, int i, void *data, int dsize);
void list_clear_(list_base_t *base);
list_iter_t list_iter_(list_base_t *base);
void* list_next_(list_iter_t *iter);

typedef list_t(int)   list_int_t;
typedef list_t(char)  list_char_t;
typedef list_t(char*) list_str_t;
typedef list_t(float) list_float_t;

/*************
 * Utils
 *************/

void tic_utils_generate_uuid(char *out, int size);

#endif // TICO_UTILS_H

#if defined(TICO_UTILS_IMPLEMENTATION)

static list_node_t *new_list_node(void *data, int dsize) {
	list_node_t *node;
	node = malloc(sizeof(*node) + dsize);
	node->data = ((char*) (node + 1));
	memcpy(node->data, data, dsize);
	node->next = NULL;
	return node;
}

void list_push_(list_base_t *base, void *val, int dsize) {
	if (!val) return;
	if (!base->root) {
		base->root = new_list_node(val, dsize);
		base->size++;
		base->last = base->root;
		return;
	}

	list_node_t *node = base->root;
	while(node->next)
		node = node->next;

	list_node_t *nnode = new_list_node(val, dsize);
	node->next = nnode;
	base->size++;
	base->last = nnode;
}

void* list_pop_(list_base_t *base) {
	list_node_t *node = base->root;
	if (!node) return NULL;
	if (!node->next) {
		base->root = NULL;
		base->last = NULL;
		void *data = node->data;

		free(node);
		base->size--;
		return data;
	}

	while (node->next->next) {
		node = node->next;
	}

	list_node_t *dnode = node->next;
	void *data = dnode->data;
	node->next = dnode->next;

	free(dnode);
	base->size--;
	base->last = dnode->next;

	return data;
}

void *list_last_(list_base_t *base) {
	return base->last;
}

list_iter_t list_iter_(list_base_t *base) {
	list_iter_t iter;
	iter.node = base->root;
	iter.data = NULL;
	iter.index = -1;
	return iter;
}

void* list_next_(list_iter_t *iter) {
	list_node_t *node = iter->node;
	void *data = NULL;
	if (node) {
		data = node->data;
		iter->data = node->data;
		iter->index++;
		iter->node = node->next;
	}

	return data;
}

/*************
 * Utils
 *************/

void tic_utils_generate_uuid(char *out, int size) {
	out[0] = '\0';
	for (int i = 0; i < size; i++) {
		char ch[1];
		sprintf(ch, "%x", rand() % 16);
		strcat(out, ch);
		// TRACELOG("%s %s", out, ch);
	}
}

#endif