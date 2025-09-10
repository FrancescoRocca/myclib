#ifndef MYCLIB_VECTOR_H
#define MYCLIB_VECTOR_H

#include <stddef.h>
#include <stdint.h>
#include <threads.h>

/**
 * @brief Vector structure.
 */
typedef struct vec {
	void *data;		  /**< Pointer to raw data array */
	size_t elem_size; /**< Size of each element in bytes */
	size_t size;	  /**< Number of elements currently stored */
	size_t capacity;  /**< Allocated capacity (number of elements) */
	mtx_t lock;		  /**< Mutex for thread safety */
} vec_s;

/**
 * @brief Create a new vector.
 *
 * @param initial_capacity Initial number of elements to allocate.
 * @param element_size Size of each element in bytes.
 * @return Pointer to the new vector, or NULL on failure.
 */
vec_s *vec_new(size_t initial_capacity, size_t element_size);

/**
 * @brief Get the number of elements stored in the vector.
 *
 * @param vec Vector.
 * @return Number of elements, or 0 if NULL.
 */
size_t vec_size(vec_s *vec);

/**
 * @brief Get the allocated capacity of the vector.
 *
 * @param vec Vector.
 * @return Capacity (number of elements), or 0 if NULL.
 */
size_t vec_cap(vec_s *vec);

/**
 * @brief Shrink the allocated memory to fit the current size.
 *
 * @param vec Vector.
 * @return 0 on success, -1 on failure.
 */
int vec_shrink(vec_s *vec);

/**
 * @brief Push a new element at the end of the vector.
 *
 * @param vec Vector.
 * @param elem Pointer to the element to add.
 * @return 0 on success, -1 on failure.
 */
int vec_push(vec_s *vec, void *elem);

/**
 * @brief Pop the last element from the vector.
 *
 * @param vec Vector.
 * @return Pointer to the removed element or NULL on failure.
 * @note Free after use.
 */
void *vec_pop(vec_s *vec);

/**
 * @brief Insert an element at a specific position.
 *
 * @param vec Vector.
 * @param index Position where to insert.
 * @param value Pointer to the element to insert.
 * @return 0 on success, -1 on failure.
 */
int vec_insert(vec_s *vec, size_t index, void *value);

/**
 * @brief Remove an element at a specific position.
 *
 * @param vec Vector.
 * @param index Index of the element to remove.
 * @return 0 on success, -1 on failure.
 */
int vec_remove(vec_s *vec, size_t index);

/**
 * @brief Get a copy of an element at the given position.
 *
 * @param vec Vector.
 * @param index Index of the element.
 * @return Pointer to the element or NULL on failure.
 * @note Free after use.
 */
void *vec_get(vec_s *vec, size_t index);

/**
 * @brief Set the value of an element at the given position.
 *
 * @param vec Vector.
 * @param index Index of the element.
 * @param value Pointer to the new value.
 * @return 0 on success, -1 on failure.
 */
int vec_set(vec_s *vec, size_t index, void *value);

/**
 * @brief Clear the vector without freeing its memory.
 *
 * @param vec Vector.
 * @return 0 on success, -1 on failure.
 */
int vec_clear(vec_s *vec);

/**
 * @brief Iterate over all elements of the vector.
 *
 * @param vec Vector.
 * @param callback Receives index and element pointer.
 * @return 0 on success, -1 on failure.
 */
int vec_foreach(vec_s *vec, void (*callback)(size_t index, void *elem));

/**
 * @brief Sort the vector using qsort().
 *
 * @param vec Vector.
 * @param cmp Comparison function.
 * @return 0 on success, -1 on failure.
 */
int vec_sort(vec_s *vec, int (*cmp)(const void *a, const void *b));

/**
 * @brief Free the vector and its resources.
 *
 * @param vec Vector.
 */
void vec_free(vec_s *vec);

#endif /* MYCLIB_VECTOR_H */
