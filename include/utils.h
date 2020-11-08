#ifndef TICO_UTILS_H
#define TICO_UTILS_H

/******************
 * Stack
 ******************/

typedef struct stack_node_t stack_node_t;

struct stack_node_t {
	void *data;
	stack_node_t *next;
};

typedef struct stack_base_t {
	int size;
} stack_base_t;

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
	list_get_(&(l)->base, i)

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

void tico_utils_generate_uuid(char *out, int size);
float tico_utils_get_ratio(float w0, float h0, float w1, float h1);
int tico_utils_hash(const char *key);

char *tico_replace_char(char *str, unsigned char find, unsigned char replace);
unsigned char* tico_utf8_codepoint(unsigned char *p, int* codepoint);
void tico_utf8_encode(unsigned char* c, int codepoint);
int tico_utf8_decode(const unsigned char *p);
void tico_write_buffer_header(const char *name, const char *text, long int size);
void tico_set_clipboard(const char *text);
const char* tico_get_clipboard();

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

void* list_get_(list_base_t *base, int i) {
	if (!base->root) return NULL;
	list_node_t *node = base->root;
	int index = 0;
	while (node) {
		if (i == index) return node;
		node = node->next;
		index++;
	}

	return NULL;
}

void list_clear_(list_base_t *base) {
	list_node_t *node = base->root;
	list_node_t *rm_node = NULL;
	if (!node) return;
	if (!node->next) {
		free(node);
		base->root = NULL;
		base->size = 0;
		return;
	}
	while (node->next) {
		rm_node = node;
		node = node->next;
		free(rm_node);
	}
	base->root = NULL;
	base->size = 0;
}

/*************
 * Utils
 *************/

#define MAXUNICODE 0x10FFFF

void tico_utils_generate_uuid(char *out, int size) {
	out[0] = '\0';
	for (int i = 0; i < size; i++) {
		char ch[2];
		memset(ch, 0, 2);
		sprintf(ch, "%x", rand() % 16);
		// ch[1] = '\0';
		strcat(out, ch);
		// TRACELOG("%s %s", out, ch);
	}
}

int tico_utils_hash(const char *key) {
	const char *p = key;
	int hash = 0;
	while (p) {
		hash <<= 1;
		hash += *p * 5913;
		p++;
	}

	return hash;
}

float tico_utils_get_ratio(float w0, float h0, float w1, float h1) {
	float ratio = tico_min(w1/w0, h1/h0);

	return ratio;
}

char *tico_replace_char(char *str, unsigned char find, unsigned char replace);
unsigned char* tico_utf8_codepoint(unsigned char *p, int* codepoint) {
	unsigned char *n;
  unsigned char c = p[0];
  if (c < 0x80) {
    *codepoint = c;
    n = p + 1;
    return n;
  }

  switch (c & 0xf0) {
    case 0xf0: {
      *codepoint = ((p[0] & 0x07) << 18) | ((p[1] & 0x3f) << 12) | ((p[2] & 0x3f) << 6) | ((p[3] & 0x3f));
      n = p + 4;
      break;
    }
    case 0xe0: {
      *codepoint = ((p[0] & 0x0f) << 12) | ((p[1] & 0x3f) << 6) | ((p[2] & 0x3f));
      n = p + 3;
      break;
    }
    case 0xc0:
    case 0xd0: {
      *codepoint = ((p[0] & 0x1f) << 6) | ((p[1] & 0x3f));
      n = p + 2;
      break;
    }
    default:
    {
      *codepoint = -1;
      n = n + 1;
    }
  }

  if (*codepoint > MAXUNICODE) *codepoint = -1;

  return n;
}
void tico_utf8_encode(unsigned char* c, int codepoint);
int tico_utf8_decode(const unsigned char *p);
void tico_write_buffer_header(const char *name, const char *text, long int size);
void tico_set_clipboard(const char *text);
const char* tico_get_clipboard();


#endif