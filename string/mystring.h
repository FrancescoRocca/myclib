#ifndef MYCLIB_STRING_H
#define MYCLIB_STRING_H

#include <stddef.h>
#include <threads.h>

/**
 * @brief Thread-safe dynamic string structure.
 */
typedef struct mcl_string {
	char *data;		 /**< Pointer to string data (null-terminated) */
	size_t size;	 /**< Current length of the string (excluding null terminator) */
	size_t capacity; /**< Allocated capacity including null terminator */
	mtx_t lock;		 /**< Mutex for thread safety */
} mcl_string_s;

/**
 * @brief Create a new string initialized with the given text.
 *
 * @param text Initial text (must not be NULL)
 * @param initial_capacity Initial buffer capacity in bytes (including null terminator).
 *                         Pass 0 to auto-calculate the capacity.
 * @return Pointer to new string on success, or NULL on failure.
 *
 * @note Caller must release the string using mcl_string_free().
 */
mcl_string_s *mcl_string_new(const char *text, size_t initial_capacity);

/**
 * @brief Append text to the string.
 *
 * @param string String to modify
 * @param text Text to append (must not be NULL, can be empty)
 * @return 0 on success, -1 on failure
 *
 * @note On failure, the original string remains unchanged.
 */
int mcl_string_append(mcl_string_s *string, const char *text);

/**
 * @brief Free the string and its resources.
 *
 * @param string String to free (safe to call with NULL)
 *
 * @note Caller must ensure no other thread is concurrently using this mcl_string.
 */
void mcl_string_free(mcl_string_s *string);

/**
 * @brief Get the current length of the string.
 *
 * @param string String to query
 * @return Length of the string (excluding null terminator), or 0 if NULL
 */
size_t mcl_string_length(mcl_string_s *string);

/**
 * @brief Get the total allocated capacity of the string buffer.
 *
 * @param string String to query
 * @return Capacity (in bytes, including null terminator), or 0 if NULL
 */
size_t mcl_string_capacity(mcl_string_s *string);

/**
 * @brief Get a pointer to a null-terminated C-string representing the content.
 *
 * @param string String to read
 * @return Pointer to a thread-local buffer containing the string, or NULL on failure.
 *
 * @note The returned pointer is valid until the next call to mcl_string_cstr()
 *       in the same thread or until the thread exits. **Do NOT free** the returned pointer.
 */
char *mcl_string_cstr(mcl_string_s *string);

#endif /* MYCLIB_STRING_H */
