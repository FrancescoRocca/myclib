#include "myset.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static int set_find_index_locked(set_s *set, void *elem, size_t *index_out) {
	size_t size = vec_size(set->data);

	for (size_t i = 0; i < size; ++i) {
		void *current = vec_get(set->data, i);
		if (current == NULL) {
			return -1;
		}

		int equal = (memcmp(current, elem, set->elem_size) == 0);
		free(current);

		if (equal) {
			*index_out = i;
			return 0;
		}
	}

	*index_out = SIZE_MAX;
	return 0;
}

set_s *set_new(size_t element_size) {
	if (element_size == 0) {
		return NULL;
	}

	set_s *set = (set_s *)malloc(sizeof(set_s));
	if (set == NULL) {
		return NULL;
	}

	if (mtx_init(&set->lock, mtx_plain) != thrd_success) {
		free(set);
		return NULL;
	}

	set->data = vec_new(8, element_size);
	if (set->data == NULL) {
		mtx_destroy(&set->lock);
		free(set);
		return NULL;
	}

	set->elem_size = element_size;

	return set;
}

int set_add(set_s *set, void *elem) {
	if (set == NULL || elem == NULL) {
		return -1;
	}

	if (mtx_lock(&set->lock) != thrd_success) {
		return -1;
	}

	size_t index;
	if (set_find_index_locked(set, elem, &index) != 0) {
		mtx_unlock(&set->lock);
		return -1;
	}

	if (index != SIZE_MAX) {
		mtx_unlock(&set->lock);
		return 0;
	}

	int ret = vec_push(set->data, elem);

	mtx_unlock(&set->lock);
	return ret;
}

int set_remove(set_s *set, void *elem) {
	if (set == NULL || elem == NULL) {
		return -1;
	}

	if (mtx_lock(&set->lock) != thrd_success) {
		return -1;
	}

	size_t index;
	if (set_find_index_locked(set, elem, &index) != 0) {
		mtx_unlock(&set->lock);
		return -1;
	}

	if (index == SIZE_MAX) {
		mtx_unlock(&set->lock);
		return -1;
	}

	int ret = vec_remove(set->data, index);

	mtx_unlock(&set->lock);
	return ret;
}

int set_clear(set_s *set) {
	if (set == NULL) {
		return -1;
	}

	if (mtx_lock(&set->lock) != thrd_success) {
		return -1;
	}

	int ret = vec_clear(set->data);

	mtx_unlock(&set->lock);
	return ret;
}

int set_contains(set_s *set, void *elem) {
	if (set == NULL || elem == NULL) {
		return -1;
	}

	if (mtx_lock(&set->lock) != thrd_success) {
		return -1;
	}

	size_t index;
	if (set_find_index_locked(set, elem, &index) != 0) {
		mtx_unlock(&set->lock);
		return -1;
	}

	mtx_unlock(&set->lock);

	return (index == SIZE_MAX) ? 0 : 1;
}

void set_free(set_s *set) {
	if (set == NULL) {
		return;
	}

	vec_free(set->data);
	mtx_destroy(&set->lock);
	free(set);
}
