#ifndef MYCLIB_STACK_H
#define MYCLIB_STACK_H

#include <stddef.h>
#include <threads.h>

#include "../vector/myvector.h"

typedef struct stack {
	vec_s *data;
	size_t elem_size;
	mtx_t lock;
} stack_s;

stack_s *stack_new(size_t initial_capacity, size_t element_size);

void *stack_pop(stack_s *stack);

int stack_push(stack_s *stack, void *elem);

void *stack_top(stack_s *stack);

void stack_free(stack_s *stack);

#endif
