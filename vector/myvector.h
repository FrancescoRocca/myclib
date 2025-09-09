#ifndef MYCLIB_VECTOR_H
#define MYCLIB_VECTOR_H

#include <stddef.h>
#include <stdint.h>
#include <threads.h>

typedef struct vec {
	void *data;
	size_t elem_size;
	size_t size;
	size_t capacity;
	mtx_t lock;
} vec_s;

vec_s *vec_new(size_t initial_capacity, size_t element_size);

size_t vec_size(vec_s *vec);

size_t vec_cap(vec_s *vec);

int vec_shrink(vec_s *vec);

int vec_push(vec_s *vec, void *elem);

// free the returned value
void *vec_pop(vec_s *vec);

int vec_insert(vec_s *vec, size_t index, void *value);

int vec_remove(vec_s *vec, size_t index);

// free the returned value
void *vec_get(vec_s *vec, size_t index);

int vec_set(vec_s *vec, size_t index, void *value);

int vec_clear(vec_s *vec);

int vec_foreach(vec_s *vec, void (*fefn)(size_t index, void *elem));

// void *vec_sort(vec_s *, void (* sortfn)(void *a, void *b))

void vec_free(vec_s *vec);

#endif
