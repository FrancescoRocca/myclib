#ifndef MYCLIB_HASHMAP_H
#define MYCLIB_HASHMAP_H

#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <threads.h>

/**< Number of buckets in the hash map */
#define MYCLIB_HASHMAP_SIZE 1024

/**
 * @brief A single bucket in the hash map.
 */
typedef struct bucket {
	void *key;			 /**< Pointer to the key */
	void *value;		 /**< Pointer to the value */
	struct bucket *next; /**< Pointer to the next bucket in case of collision */
} bucket_s;

/**
 * @brief Function pointer type for a hash function.
 *
 * @param[in] key Pointer to the key to hash.
 * @return The computed hash as an unsigned integer.
 */
typedef unsigned int hash_f(const void *key);

/**
 * @brief Function pointer type for a key comparison function.
 *
 * @param[in] key_a Pointer to the first key.
 * @param[in] key_b Pointer to the second key.
 * @return true if the keys are considered equal, false otherwise.
 */
typedef bool equal_f(const void *key_a, const void *key_b);

/**
 * @brief Function pointer type for freeing a key.
 *
 * @param[in] key Pointer to the key to free.
 */
typedef void free_key_f(void *key);

/**
 * @brief Function pointer type for freeing a value.
 *
 * @param[in] value Pointer to the value to free.
 */
typedef void free_value_f(void *value);

/**
 * @brief Main structure representing the hash map.
 * Thread-safe for concurrent operations on different keys.
 */
typedef struct hashmap {
	hash_f *hash;					   /**< Hash function */
	equal_f *equal;					   /**< Equality comparison function */
	free_key_f *free_key;			   /**< Key deallocation function (optional) */
	free_value_f *free_value;		   /**< Value deallocation function (optional) */
	size_t key_size;				   /**< Size in bytes of the key */
	size_t value_size;				   /**< Size in bytes of the value */
	bucket_s map[MYCLIB_HASHMAP_SIZE]; /**< Array of bucket chains */
	atomic_size_t size;				   /**< Hashmap size (number of keys) - atomic */
	mtx_t *locks;					   /**< Mutex array */
	size_t num_locks;				   /**< Number of mutex */
} hashmap_s;

/**
 * @brief Initialize a new hash map.
 *
 * @param[in] hash Function used to hash keys.
 * @param[in] equal Function used to compare keys.
 * @param[in] free_key Function used to free keys (optional, can be NULL).
 * @param[in] free_value Function used to free values (optional, can be NULL).
 * @param[in] key_size Size in bytes of each key to be stored.
 * @param[in] value_size Size in bytes of each value to be stored.
 * @return A pointer to the newly initialized hash map, or NULL on failure.
 */
hashmap_s *hm_new(hash_f *hash, equal_f *equal, free_key_f *free_key, free_value_f *free_value,
				  size_t key_size, size_t value_size);

/**
 * @brief Free all resources used by the hash map.
 *
 * @param[in] hashmap Hashmap.
 */
void hm_free(hashmap_s *hashmap);

/**
 * @brief Free a bucket returned by get.
 *
 * @param[in] bucket Pointer to the bucket
 */
void hm_free_bucket(bucket_s *bucket);

/**
 * @brief Insert or update a key-value pair in the hash map.
 *
 * If the key already exists, the old value is freed (if free_value_fn is provided)
 * and replaced with the new value. If the key doesn't exist, a new entry is created.
 * Both key and value are copied into the hashmap using memcpy.
 *
 * @param[in] hashmap Pointer to the hash map.
 * @param[in] key Pointer to the key to insert (will be copied, must not be NULL).
 * @param[in] value Pointer to the value to insert (will be copied, must not be NULL).
 * @return true if the operation succeeded, false on failure (NULL hashmap/key/value or memory
 * allocation failure).
 */
bool hm_set(hashmap_s *hashmap, void *key, void *value);

/**
 * @brief Retrieve a bucket by key.
 *
 * @param[in] hashmap Pointer to the hash map.
 * @param[in] key Pointer to the key to search for.
 * @return Pointer to the copy of the bucket or NULL on failure.
 * @note Free after use with hm_free_bucket().
 */
bucket_s *hm_get(hashmap_s *hashmap, void *key);

/**
 * @brief Remove a key-value pair from the hash map.
 *
 * Searches for the given key and removes it from the hash map. Both the key
 * and value are freed using the provided free functions (if not NULL).
 *
 * @param[in] hashmap Pointer to the hash map.
 * @param[in] key Pointer to the key to remove.
 * @return true if the key was found and removed, false otherwise.
 */
bool hm_remove(hashmap_s *hashmap, void *key);

/**
 * @brief Get the number of entries in the hash map.
 *
 * @param[in] hashmap Pointer to the hash map.
 * @return Number of key-value pairs in the map.
 */
size_t hm_size(hashmap_s *hashmap);

/**
 * @brief Check if a key exists in the hash map.
 *
 * @param[in] hashmap Pointer to the hash map.
 * @param[in] key Pointer to the key to search for.
 * @return true if the key exists, false otherwise.
 */
bool hm_contains(hashmap_s *hashmap, void *key);

/**
 * @brief Iterate over all entries in the hash map.
 *
 * @param[in] hashmap Pointer to the hash map.
 * @param[in] callback Function called for each bucket.
 */
void hm_foreach(hashmap_s *hashmap, void (*callback)(bucket_s *bucket));

/**
 * @brief Remove all entries from the hash map.
 *
 * @param[in] hashmap Pointer to the hash map.
 */
void hm_clear(hashmap_s *hashmap);

#endif /* MYCLIB_HASHMAP_H */
