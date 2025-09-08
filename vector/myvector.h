#ifndef MYCLIB_VECTOR_H
#define MYCLIB_VECTOR_H

#include <stddef.h>
#include <stdint.h>
#include <threads.h>

typedef struct vector {
	void *data;
	size_t elem_size;
	size_t size;
	size_t capacity;
	mtx_t lock;
} vector_s;

vector_s *vector_new(size_t initial_capacity, size_t element_size);
int vector_push(vector_s *vec, void *elem);
size_t vector_size(vector_s *vec);
size_t vector_cap(vector_s *vec);

void *vector_get(vector_s *vec, size_t index);

void vector_free(vector_s *vec);

#endif
