#ifndef MYCLIB_STRING_H
#define MYCLIB_STRING_H

#include <stddef.h>
#include <threads.h>

/**
 * @brief Thread-safe dynamic string structure.
 */
typedef struct mcl_string {
	char *data;		 /**< Pointer to null-terminated string data */
	size_t size;	 /**< Current length (excluding null terminator) */
	size_t capacity; /**< Allocated capacity including null terminator */
	mtx_t lock;		 /**< Mutex for thread safety */
} mcl_string_s;

/**
 * @brief Create a new string initialized with the given text.
 *
 * @param text Initial text.
 * @param initial_capacity Initial buffer capacity (including null terminator). Pass 0 to auto-calculate.
 * @return Pointer to the new string, or NULL on failure.
 */
mcl_string_s *mcl_string_new(const char *text, size_t initial_capacity);

/**
 * @brief Free the string and its resources.
 *
 * @param string String to free (safe to call with NULL).
 */
void mcl_string_free(mcl_string_s *string);

/**
 * @brief Append text to the string.
 *
 * @param string String to modify.
 * @param text Text to append.
 * @return 0 on success, -1 on failure.
 */
int mcl_string_append(mcl_string_s *string, const char *text);

/**
 * @brief Extend by adding another string.
 *
 * @param destination Destination string.
 * @param source Source string.
 * @return 0 on success, -1 on failure.
 */
int mcl_string_extend(mcl_string_s *destination, mcl_string_s *source);

/**
 * @brief Clear the string content without freeing the memory.
 *
 * @param string String to clear.
 */
void mcl_string_clear(mcl_string_s *string);

/**
 * @brief Get the current length of the string.
 *
 * @param string String to query.
 * @return Length excluding null terminator, or 0 if NULL.
 */
size_t mcl_string_length(mcl_string_s *string);

/**
 * @brief Get the total allocated capacity of the string buffer.
 *
 * @param string String to query.
 * @return Capacity in bytes (including null terminator), or 0 if NULL.
 */
size_t mcl_string_capacity(mcl_string_s *string);

/**
 * @brief Get a pointer to a null-terminated C-string.
 *
 * @param string String to read.
 * @return Pointer to a thread-local buffer, or NULL on failure.
 *
 * @note Valid until the next call in the same thread. Do NOT free the returned pointer.
 * Do NOT call more than once this function in a print function.
 */
char *mcl_string_cstr(mcl_string_s *string);

/**
 * @brief Compare two strings.
 *
 * @param s1 First string.
 * @param s2 Second string.
 * @return -123 on failure or same as strcmp().
 */
int mcl_string_compare(mcl_string_s *s1, mcl_string_s *s2);

/**
 * @brief Convert the string to uppercase.
 *
 * @param string String to modify.
 */
void mcl_string_toupper(mcl_string_s *string);

/**
 * @brief Convert the string to lowercase.
 *
 * @param string String to modify.
 */
void mcl_string_tolower(mcl_string_s *string);

#endif /* MYCLIB_STRING_H */
