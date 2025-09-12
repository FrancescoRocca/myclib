#ifndef MYCLIB_SET_H
#define MYCLIB_SET_H

// TODO: WIP

#include <threads.h>

#include "../hashmap/myhashmap.h"

typedef struct set {
	hashmap_s *map;
	mtx_t lock;
} set_s;

set_s *set_new(size_t element_size);

int set_add(set_s *set, void *elem);

int set_remove(set_s *set, void *elem);

int set_clear(set_s *set);

int set_contains(set_s *set, void *elem);

void set_free(set_s *set);

#endif
