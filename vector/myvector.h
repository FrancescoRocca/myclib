#ifndef MYCLIB_VECTOR_H
#define MYCLIB_VECTOR_H

#include <stddef.h>
#include <stdint.h>
#include <threads.h>

typedef struct {
	void *data;
	size_t elem_size;
	size_t size;
	size_t capacity;
	mtx_t lock;
} mcl_vector_s;

mcl_vector_s *mcl_vector_new(size_t initial_capacity, size_t element_size);
int mcl_vector_push(mcl_vector_s *vec, void *elem);
size_t mcl_vector_size(mcl_vector_s *vec);
size_t mcl_vector_capacity(mcl_vector_s *vec);

void *mcl_vector_get(mcl_vector_s *vec, size_t index);

void mcl_vector_free(mcl_vector_s *vec);

#endif
