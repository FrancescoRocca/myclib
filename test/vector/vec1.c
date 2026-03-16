#include "../vector/myvector.h"
#include <assert.h>
#include <stdlib.h>

typedef struct my_elem {
	char name[32];
	int age;
} my_elem_s;

/* Functions used to iterate for each vector's element */
static void multiply(size_t index, void *elem) {
	(void)index;
	my_elem_s *e = (my_elem_s *)elem;
	e->age = e->age * 2;
}

/* Another way to use foreach
static void print(size_t index, void *elem) {
	my_elem_s *e = (my_elem_s *)elem;
	printf("%s (%d)\n", e->name, e->age);
}
*/

/* Compare function used to sort */
int my_cmp(const void *a, const void *b) {
	/* Sort by age */
	my_elem_s *ma = (my_elem_s *)a;
	my_elem_s *mb = (my_elem_s *)b;

	return ma->age - mb->age;
}

int main(void) {
	/* Allocate a new vector */
	size_t elem_size = sizeof(my_elem_s);
	vec_s *v = vec_new(10, elem_size);
	assert(vec_size(v) == 0);
	assert(vec_cap(v) == 16);

	/* Push an element */
	my_elem_s e1 = {
		.age = 21,
		.name = "John",
	};
	vec_push(v, &e1);
	assert(vec_size(v) == 1);

	/* Retrieve an element (Remember to free) */
	my_elem_s *e1_v = (my_elem_s *)vec_get(v, 0);
	free(e1_v);

	/* Pop last element (Remember to free) */
	my_elem_s *pop = (my_elem_s *)vec_pop(v);
	free(pop);

	/* Insert an element */
	vec_push(v, &e1);
	e1.age = 25;
	vec_push(v, &e1);
	e1.age = 19;
	vec_insert(v, 2, &e1);
	my_elem_s last = {
		.age = 33,
		.name = "Last",
	};
	vec_push(v, &last);
	my_elem_s *lastv = (my_elem_s *)vec_pop(v);
	free(lastv);
	assert(vec_get(v, vec_size(v)) == NULL);
	assert(vec_remove(v, vec_size(v)) == -1);

	/* Remove from the middle and validate shifted content for elem_size > 1 */
	assert(vec_remove(v, 1) == 0);
	my_elem_s *shifted = (my_elem_s *)vec_get(v, 1);
	assert(shifted != NULL);
	assert(shifted->age == 19);
	free(shifted);

	my_elem_s replacement = {
		.age = 40,
		.name = "Updated",
	};
	assert(vec_set(v, 1, &replacement) == 0);
	assert(vec_set(v, vec_size(v), &replacement) == -1);
	my_elem_s *updated = (my_elem_s *)vec_get(v, 1);
	assert(updated != NULL);
	assert(updated->age == 40);
	free(updated);

	/* Iterate for each element */
	vec_foreach(v, multiply);
	/* Print each element */
	// vec_foreach(v, print);

	/* Shrink to fit current size and ensure data is still readable */
	size_t before_shrink_size = vec_size(v);
	assert(vec_shrink(v) == 0);
	assert(vec_cap(v) == before_shrink_size);
	my_elem_s *shrink_elem = (my_elem_s *)vec_get(v, 1);
	assert(shrink_elem != NULL);
	free(shrink_elem);

	/* Sort */
	vec_sort(v, my_cmp);

	/* Deallocate the vector */
	vec_free(v);
}
