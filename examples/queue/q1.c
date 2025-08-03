#include <assert.h>
#include <stdio.h>

#include "../../queue/myqueue.h"

int main(void) {
	mcl_queue *queue = mcl_queue_init(3, sizeof(int));
	assert(queue != NULL);

	int val, out;

	val = 1;
	assert(mcl_queue_push(queue, &val) == 0);

	val = 2;
	assert(mcl_queue_push(queue, &val) == 0);

	int front = *((int *)mcl_queue_get_front(queue));
	int rear = *((int *)mcl_queue_get_rear(queue));
	printf("Front: %d, Rear: %d\n", front, rear);
	assert(front == 1);
	assert(rear == 2);

	assert(mcl_queue_pop(queue, &out) == 0);
	printf("Pop: %d\n", out);
	assert(out == 1);

	front = *((int *)mcl_queue_get_front(queue));
	printf("Front after pop: %d\n", front);
	assert(front == 2);

	val = 3;
	assert(mcl_queue_push(queue, &val) == 0);

	rear = *((int *)mcl_queue_get_rear(queue));
	printf("Rear after push 3: %d\n", rear);
	assert(rear == 3);

	val = 4;
	int res = mcl_queue_push(queue, &val);
	assert(res == 0);

	while (mcl_queue_pop(queue, &out) == 0) {
		printf("Pop: %d\n", out);
	}
	printf("Queue is now empty\n");

	assert(mcl_queue_pop(queue, &out) == -1);

	mcl_queue_free(queue);
	printf("All tests passed successfully.\n");
	return 0;
}
