#ifndef MYCLIB_STACK_H
#define MYCLIB_STACK_H

#include <stddef.h>
#include <threads.h>

#include "../vector/myvector.h"

/**
 * @brief Stack (LIFO) structure.
 */
typedef struct stack {
	vec_s *data;	  /**< Internal vector storage */
	size_t elem_size; /**< Size of each element in bytes */
	mtx_t lock;		  /**< Mutex for thread-safety */
} stack_s;

/**
 * @brief Create a new stack.
 *
 * @param initial_capacity Initial capacity (number of elements).
 * @param element_size Size of each element in bytes.
 * @return Pointer to the new stack, or NULL on failure.
 */
stack_s *stack_new(size_t initial_capacity, size_t element_size);

/**
 * @brief Pop the top element of the stack.
 *
 * @param stack Stack.
 * @return Pointer to the removed element or NULL on failure.
 * @note Free after use.
 */
void *stack_pop(stack_s *stack);

/**
 * @brief Push an element on top of the stack.
 *
 * @param stack Stack.
 * @param elem Pointer to the element to push.
 * @return 0 on success, -1 on failure.
 */
int stack_push(stack_s *stack, void *elem);

/**
 * @brief Get a copy of the top element without removing it.
 *
 * @param stack Stack.
 * @return Pointer to the element or NULL on failure.
 * @note Free after use.
 */
void *stack_top(stack_s *stack);

/**
 * @brief Free the stack and its resources.
 *
 * @param stack Stack.
 */
void stack_free(stack_s *stack);

#endif /* MYCLIB_STACK_H */
