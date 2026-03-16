#ifndef MYCLIB_SET_H
#define MYCLIB_SET_H

#include <stddef.h>
#include <threads.h>

#include "../vector/myvector.h"

typedef struct set {
	vec_s *data;
	size_t elem_size;
	mtx_t lock;
} set_s;

/* Create a new set for fixed-size elements. */
set_s *set_new(size_t element_size);

/* Add a new element to the set (no-op if already present). */
int set_add(set_s *set, void *elem);

/* Remove an element from the set. */
int set_remove(set_s *set, void *elem);

/* Remove all elements from the set. */
int set_clear(set_s *set);

/* Returns 1 if present, 0 if absent, -1 on error. */
int set_contains(set_s *set, void *elem);

/* Free the set and all related resources. */
void set_free(set_s *set);

#endif
