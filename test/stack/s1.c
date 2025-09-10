#include <assert.h>
#include <stdlib.h>

#include "../../stack/mystack.h"

void test_s1(void) {
	stack_s *stack = stack_new(32, sizeof(int));
	int num = 10;
	stack_push(stack, &num);

	num = 13;
	stack_push(stack, &num);

	int *rv = (int *)stack_top(stack);
	assert(*rv == 13);
	free(rv);

	rv = (int *)stack_pop(stack);
	assert(*rv == 13);
	free(rv);

	stack_free(stack);
}
