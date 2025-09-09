#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../vector/myvector.h"

typedef struct my_elem {
	char name[32];
	int age;
} my_elem_s;

/* Functions used to iterate for each vector's element */
static void multiply(void *elem) {
	my_elem_s *e = (my_elem_s *)elem;
	e->age = e->age * 2;
}

/* Another way to use foreach
static void print(void *elem) {
	my_elem_s *e = (my_elem_s *)elem;
	printf("%s (%d)\n", e->name, e->age);
}
*/

void test_v1() {
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
	vec_push(v, &e1);
	vec_insert(v, 2, &e1);
	my_elem_s last = {
		.age = 33,
		.name = "Last",
	};
	vec_push(v, &last);
	my_elem_s *lastv = (my_elem_s *)vec_pop(v);
	free(lastv);

	/* Iterate for each element */
	vec_foreach(v, multiply);
	/* Print each element */
	// vec_foreach(v, print);

	/* Deallocate the vector */
	vec_free(v);
}
