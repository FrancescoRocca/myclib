#ifndef MYCLIB_STRING_H
#define MYCLIB_STRING_H

#include <pthread.h>
#include <stddef.h>

/**
 * @brief Thread-safe dynamic string structure.
 */
typedef struct mcl_string_t {
	size_t size;		  /**< Current length of the string (excluding null terminator) */
	size_t capacity;	  /**< Allocated capacity including null terminator */
	char *data;			  /**< Pointer to string data (null-terminated) */
	pthread_mutex_t lock; /**< Mutex for thread safety */
} mcl_string;

/**
 * @brief Create a new string initialized with the given text.
 *
 * @param text Initial text (must not be NULL)
 * @param initial_capacity Initial buffer capacity (0 to auto-calculate)
 * @return Pointer to new string on success, or NULL on failure.
 *
 * @note Caller must release the string using mcl_string_free().
 */
mcl_string *mcl_string_new(const char *text, size_t initial_capacity);

/**
 * @brief Append text to the string.
 *
 * @param string String to modify
 * @param text Text to append (must not be NULL, can be empty)
 * @return 0 on success, -1 on failure
 *
 * @note On failure, the original string remains unchanged.
 */
int mcl_string_append(mcl_string *string, const char *text);

/**
 * @brief Free the string and its resources.
 *
 * @param string String to free (safe to call with NULL)
 */
void mcl_string_free(mcl_string *string);

/**
 * @brief Get the current length of the string.
 *
 * @param string String to query
 * @return Length of the string (excluding null terminator), or 0 if NULL
 */
size_t mcl_string_length(mcl_string *string);

/**
 * @brief Get the total allocated capacity of the string buffer.
 *
 * @param string String to query
 * @return Capacity (in bytes, including null terminator), or 0 if NULL
 */
size_t mcl_string_capacity(mcl_string *string);

/**
 * @brief Get a copy of the string as a null-terminated C-string.
 *
 * @param string String to copy
 * @return Newly allocated copy of the string, or NULL on failure
 *
 * @warning Caller is responsible for freeing the returned pointer.
 */
char *mcl_string_cstr(mcl_string *string);

#endif /* MYCLIB_STRING_H */
