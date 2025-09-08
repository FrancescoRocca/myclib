#include "myvector.h"

#include <stdlib.h>
#include <string.h>
#include <threads.h>

/* Returns the next power of two of a number */
static size_t next_power_two(size_t len) {
	if (len == 0) return 1;

	size_t p = 1;
	while (p < len) {
		if (p > SIZE_MAX / 2) {
			p = len;
			break;
		}
		p <<= 1;
	}

	return p;
}

vector_s *vector_new(size_t initial_capacity, size_t element_size) {
	if (initial_capacity < 0) {
		return NULL;
	}

	vector_s *vec = (vector_s *)malloc(sizeof(vector_s));
	if (vec == NULL) {
		return NULL;
	}

	vec->capacity = next_power_two(initial_capacity);
	vec->elem_size = element_size;
	vec->size = 0;
	vec->data = malloc(vec->capacity * vec->elem_size);
	if (vec->data == NULL) {
		free(vec);

		return NULL;
	}

	if (mtx_init(&vec->lock, mtx_plain) != thrd_success) {
		free(vec->data);
		free(vec);

		return NULL;
	}

	return vec;
}

int vector_push(vector_s *vec, void *elem) {
	if (vec == NULL || elem == NULL) {
		return -1;
	}

	if (mtx_lock(&vec->lock) != thrd_success) {
		return -1;
	}

	if (vec->size + 1 > vec->capacity) {
		/* Reallocate buffer */
		vec->capacity = next_power_two(vec->size + 1);
		void *tmp = malloc(vec->capacity * vec->elem_size);
		if (tmp == NULL) {
			mtx_unlock(&vec->lock);

			return -1;
		}
		vec->data = tmp;
	}

	/* Add the new element */
	memcpy(vec->data + (vec->size * vec->elem_size), elem, vec->elem_size);
	vec->size++;

	mtx_unlock(&vec->lock);

	return 0;
}

void vector_free(vector_s *vec) {
	if (vec == NULL) {
		return;
	}

	if (vec->data != NULL) {
		free(vec->data);
	}

	free(vec);
}

size_t vector_size(vector_s *vec) {
	if (vec == NULL) {
		return -1;
	}

	if (mtx_lock(&vec->lock) != thrd_success) {
		return -1;
	}

	size_t size = vec->size;

	mtx_unlock(&vec->lock);

	return size;
}

size_t vector_cap(vector_s *vec) {
	if (vec == NULL) {
		return -1;
	}

	if (mtx_lock(&vec->lock) != thrd_success) {
		return -1;
	}

	size_t cap = vec->capacity;

	mtx_unlock(&vec->lock);

	return cap;
}

void *vector_get(vector_s *vec, size_t index) {
	if (vec == NULL || index < 0 || index > vec->size) {
		return NULL;
	}

	if (mtx_lock(&vec->lock) != thrd_success) {
		return NULL;
	}

	void *elem = malloc(vec->elem_size);
	if (elem == NULL) {
		return NULL;
	}

	memcpy(elem, vec->data + (index * vec->elem_size), vec->elem_size);

	mtx_unlock(&vec->lock);

	return elem;
}
