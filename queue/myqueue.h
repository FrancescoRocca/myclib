#ifndef MYCLIB_QUEUE_H
#define MYCLIB_QUEUE_H

#include <stddef.h>
#include <threads.h>

/**
 * @brief A simple circular queue (ring buffer).
 */
typedef struct mcl_queue {
	size_t front;	  /**< Index of the next element to read. */
	size_t rear;	  /**< Index where the next element will be written. */
	size_t size;	  /**< Current number of elements in the queue. */
	size_t capacity;  /**< Maximum number of elements the queue can hold. */
	size_t elem_size; /**< Size in bytes of each element. */
	void *buffer;	  /**< Memory buffer that holds the elements. */
	mtx_t lock;		  /**< Mutex to protect concurrent access. */
} mcl_queue_s;

/**
 * @brief Create and initialize a new queue.
 *
 * @param queue_size Number of elements the queue can hold.
 * @param elem_size  Size in bytes of each element.
 * @return Pointer to the new queue, or NULL on failure.
 */
mcl_queue_s *mcl_queue_new(size_t queue_size, size_t elem_size);

/**
 * @brief Add an element to the queue.
 *
 * @param queue Pointer to the queue.
 * @param elem  Pointer to the data to add.
 * @return 0 on success, -1 if the queue is full or on error.
 */
int mcl_queue_push(mcl_queue_s *queue, const void *elem);

/**
 * @brief Remove an element from the queue.
 *
 * @param queue    Pointer to the queue.
 * @param out_elem Pointer to memory where the removed element will be copied.
 * @return 0 on success, -1 if the queue is empty or on error.
 */
int mcl_queue_pop(mcl_queue_s *queue, void *out_elem);

/**
 * @brief Copy the front element without removing it.
 *
 * @param queue Pointer to the queue.
 * @param out   Pointer to memory where the element will be copied.
 * @return 0 on success, -1 if the queue is empty or on error.
 */
int mcl_queue_get_front(mcl_queue_s *queue, void *out);

/**
 * @brief Copy the last element without removing it.
 *
 * @param queue Pointer to the queue.
 * @param out   Pointer to memory where the element will be copied.
 * @return 0 on success, -1 if the queue is empty or on error.
 */
int mcl_queue_get_rear(mcl_queue_s *queue, void *out);

/**
 * @brief Free all resources used by the queue.
 *
 * @param queue Pointer to the queue to free.
 */
void mcl_queue_free(mcl_queue_s *queue);

#endif	// MYCLIB_QUEUE_H
