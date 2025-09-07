#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../vector/myvector.h"

typedef struct my_elem {
	char name[32];
	int age;
} my_elem_s;

void test_v1() {
	/* Allocate a new vector */
	size_t elem_size = sizeof(my_elem_s);
	mcl_vector_s *v = mcl_vector_new(10, elem_size);
	assert(mcl_vector_size(v) == 0);
	assert(mcl_vector_capacity(v) == 16);

	/* Push an element */
	my_elem_s e1 = {
		.age = 21,
		.name = "John",
	};
	mcl_vector_push(v, &e1);
	assert(mcl_vector_size(v) == 1);

	/* Retrieve an element (Remember to FREE) */
	my_elem_s *e1_v = (my_elem_s *)mcl_vector_get(v, 0);
	printf("name: %s, age: %d\n", e1_v->name, e1_v->age);
	free(e1_v);

	/* Deallocate the vector */
	mcl_vector_free(v);
}