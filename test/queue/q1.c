#include <assert.h>

#include "../../queue/myqueue.h"

void test_q1(void) {
	/* Allocate a new queue */
	mcl_queue_s *queue = mcl_queue_new(3, sizeof(int));
	assert(queue != NULL);

	int val, out;

	/* Push value to the ring buffer */
	val = 1;
	assert(mcl_queue_push(queue, &val) == 0);

	val = 2;
	assert(mcl_queue_push(queue, &val) == 0);

	/* Retrieve values */
	int front, rear;
	assert(mcl_queue_get_front(queue, &front) == 0);
	assert(mcl_queue_get_rear(queue, &rear) == 0);
	assert(front == 1);
	assert(rear == 2);

	/* Remove an element from the buffer */
	assert(mcl_queue_pop(queue, &out) == 0);
	assert(out == 1);

	assert(mcl_queue_get_front(queue, &front) == 0);
	assert(front == 2);

	val = 3;
	assert(mcl_queue_push(queue, &val) == 0);

	assert(mcl_queue_get_rear(queue, &rear) == 0);
	assert(rear == 3);

	val = 4;
	assert(mcl_queue_push(queue, &val) == 0);

	/* Clear queue */
	while (mcl_queue_pop(queue, &out) == 0) {}
	assert(queue->size == 0);

	/* Deallocate memory */
	mcl_queue_free(queue);
}
