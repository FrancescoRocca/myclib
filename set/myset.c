#include "myset.h"

#include <stdlib.h>

set_s *set_new(size_t element_size) {
	if (element_size == 0) {
		return NULL;
	}

	set_s *set = (set_s *)malloc(sizeof(set_s));
	if (set == NULL) {
		return NULL;
	}

	// TODO: think about how to deal the free_value for each element

	return set;
}

int set_add(set_s *set, void *elem);

int set_remove(set_s *set, void *elem);

int set_clear(set_s *set);

int set_contains(set_s *set, void *elem);

void set_free(set_s *set);
