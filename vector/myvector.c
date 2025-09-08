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

vec_s *vec_new(size_t initial_capacity, size_t element_size) {
	vec_s *vec = (vec_s *)malloc(sizeof(vec_s));
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

int vec_push(vec_s *vec, void *elem) {
	if (vec == NULL || elem == NULL) {
		return -1;
	}

	if (mtx_lock(&vec->lock) != thrd_success) {
		return -1;
	}

	if (vec->size + 1 > vec->capacity) {
		/* Reallocate buffer */
		vec->capacity = next_power_two(vec->size + 1);
		void *tmp = realloc(vec->data, vec->capacity * vec->elem_size);
		if (tmp == NULL) {
			mtx_unlock(&vec->lock);

			return -1;
		}
		vec->data = tmp;
	}

	/* Add the new element */
	memcpy((char *)vec->data + (vec->size * vec->elem_size), elem, vec->elem_size);
	vec->size++;

	mtx_unlock(&vec->lock);

	return 0;
}

void vec_free(vec_s *vec) {
	if (vec == NULL) {
		return;
	}

	if (vec->data != NULL) {
		free(vec->data);
	}

	free(vec);
}

size_t vec_size(vec_s *vec) {
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

size_t vec_cap(vec_s *vec) {
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

void *vec_get(vec_s *vec, size_t index) {
	if (vec == NULL || index > vec->size) {
		return NULL;
	}

	if (mtx_lock(&vec->lock) != thrd_success) {
		return NULL;
	}

	void *elem = malloc(vec->elem_size);
	if (elem == NULL) {
		return NULL;
	}

	memcpy(elem, (char *)vec->data + (index * vec->elem_size), vec->elem_size);

	mtx_unlock(&vec->lock);

	return elem;
}

int vec_shrink(vec_s *vec) {
	if (vec == NULL) {
		return -1;
	}

	if (mtx_lock(&vec->lock) != thrd_success) {
		return -1;
	}

	void *tmp = realloc(vec->data, vec->size);
	if (tmp == NULL) {
		mtx_unlock(&vec->lock);

		return -1;
	}

	vec->data = tmp;
	vec->capacity = vec->size;

	mtx_unlock(&vec->lock);

	return 0;
}

int vec_clear(vec_s *vec) {
	if (vec == NULL) {
		return -1;
	}

	if (mtx_lock(&vec->lock) != thrd_success) {
		return -1;
	}

	memset(vec->data, 0, vec->size);
	vec->size = 0;

	mtx_unlock(&vec->lock);

	return 0;
}

void *vec_pop(vec_s *vec) {
	if (vec == NULL) {
		return NULL;
	}

	if (vec->size == 0) {
		return NULL;
	}

	if (mtx_lock(&vec->lock) != thrd_success) {
		return NULL;
	}

	void *e = malloc(vec->elem_size);
	vec->size--;
	memcpy(e, (char *)vec->data + (vec->size * vec->elem_size), vec->elem_size);

	mtx_unlock(&vec->lock);

	return e;
}

int vec_insert(vec_s *vec, size_t index, void *value) {
	if (vec == NULL || value == NULL) {
		return -1;
	}

	if (index > vec->size) {
		return -1;
	}

	if (mtx_lock(&vec->lock) != thrd_success) {
		return -1;
	}

	if (vec->size + 1 > vec->capacity) {
		/* No space, realloc */
		void *tmp = realloc(vec->data, next_power_two(vec->size + 1));
		if (tmp == NULL) {
			mtx_unlock(&vec->lock);
			
			return -1;
		}
		vec->data = tmp;
	}

	/* Shift memory and copy the new value */
	size_t tmp_size = vec->size - index + 1;
	memmove((char *)vec->data + (index * vec->elem_size), (char *)vec->data + ((index + 1) * vec->elem_size), tmp_size * vec->elem_size);
	memcpy((char *)vec->data + (index * vec->elem_size), value, vec->elem_size);
	vec->size++;

	mtx_unlock(&vec->lock);

	return 0;
}

int vec_remove(vec_s *vec, size_t index) {
	if (vec == NULL) {
		return -1;
	}

	if (index > vec->size) {
		return -1;
	}

	if (mtx_lock(&vec->lock) != thrd_success) {
		return -1;
	}

	size_t size = vec->size - index;
	/* Overwrite bytes */
	memmove((char *)vec->data + (index * vec->elem_size), (char *)vec->data + ((index + 1) * vec->elem_size), size);
	vec->size--;

	mtx_unlock(&vec->lock);

	return 0;
}

int vec_set(vec_s *vec, size_t index, void *value) {
	if (vec == NULL || value == NULL) {
		return -1;
	}

	if (index > vec->size) {
		return -1;
	}

	if (mtx_lock(&vec->lock), thrd_success) {
		return -1;
	}

	memcpy((char *)vec->data + (index * vec->elem_size), value, vec->elem_size);

	mtx_unlock(&vec->lock);

	return 0;
}
