#include "mystack.h"

#include <stdlib.h>
#include <threads.h>

stack_s *stack_new(size_t initial_capacity, size_t element_size) {
	if (element_size == 0) {
		return NULL;
	}

	stack_s *stack = (stack_s *)malloc(sizeof(stack_s));
	if (stack == NULL) {
		return NULL;
	}

	if (mtx_init(&stack->lock, mtx_recursive) != thrd_success) {
		free(stack);
		return NULL;
	}

	/* Allocate the vec data */
	stack->data = vec_new(initial_capacity, element_size);
	if (stack->data == NULL) {
		free(stack);
		return NULL;
	}

	stack->elem_size = element_size;

	return stack;
}

void *stack_pop(stack_s *stack) {
	if (stack == NULL) {
		return NULL;
	}

	if (mtx_lock(&stack->lock) != thrd_success) {
		return NULL;
	}

	void *elem = vec_pop(stack->data);

	mtx_unlock(&stack->lock);

	return elem;
}

int stack_push(stack_s *stack, void *elem) {
	if (stack == NULL || elem == NULL) {
		return -1;
	}

	if (mtx_lock(&stack->lock) != thrd_success) {
		return -1;
	}

	int ret = vec_push(stack->data, elem);

	mtx_unlock(&stack->lock);

	return ret;
}

void *stack_top(stack_s *stack) {
	if (stack == NULL) {
		return NULL;
	}

	if (mtx_lock(&stack->lock) != thrd_success) {
		return NULL;
	}

	void *elem = vec_get(stack->data, stack->data->size - 1);

	mtx_unlock(&stack->lock);

	return elem;
}

void stack_free(stack_s *stack) {
	if (stack == NULL) {
		return;
	}

	vec_free(stack->data);
	free(stack);
}
