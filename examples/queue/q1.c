#include <stdio.h>

#include "../../queue/myqueue.h"

int main(void) {
	/* Allocate a new queue */
	/* Always remember to check return values */
	mcl_queue_s *queue = mcl_queue_new(3, sizeof(int));

	int val, out;

	/* Push value to the ring buffer */
	val = 1;
	mcl_queue_push(queue, &val);

	val = 2;
	mcl_queue_push(queue, &val);

	/* Retrieve values */
	int front, rear;
	mcl_queue_get_front(queue, &front);
	mcl_queue_get_rear(queue, &rear);
	printf("Front: %d, Rear: %d\n", front, rear);

	/* Remove an element from the buffer */
	mcl_queue_pop(queue, &out);
	printf("Pop: %d\n", out);

	mcl_queue_get_front(queue, &front);
	printf("Front after pop: %d\n", front);

	val = 3;
	mcl_queue_push(queue, &val);

	mcl_queue_get_rear(queue, &rear);
	printf("Rear after push 3: %d\n", rear);

	val = 4;
	mcl_queue_push(queue, &val);

	/* Clear queue */
	while (mcl_queue_pop(queue, &out) == 0) {
		printf("Pop: %d\n", out);
	}
	puts("Queue is now empty");

	/* Deallocate memory */
	mcl_queue_free(queue);

	return 0;
}
