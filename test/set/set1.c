#include "../set/myset.h"

#include <assert.h>

int main(void) {
	set_s *set = set_new(sizeof(int));
	assert(set != NULL);

	int a = 10;
	int b = 20;
	int another_a = 10;
	int missing = 99;

	assert(set_add(set, &a) == 0);
	assert(set_add(set, &b) == 0);
	/* Duplicate values are ignored and treated as success. */
	assert(set_add(set, &another_a) == 0);

	assert(set_contains(set, &a) == 1);
	assert(set_contains(set, &another_a) == 1);
	assert(set_contains(set, &missing) == 0);

	assert(set_remove(set, &a) == 0);
	assert(set_contains(set, &a) == 0);
	assert(set_remove(set, &a) == -1);

	assert(set_clear(set) == 0);
	assert(set_contains(set, &b) == 0);

	set_free(set);
}
