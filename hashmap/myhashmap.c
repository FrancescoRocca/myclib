#include "myhashmap.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * @brief Returns the mutex ID.
 */
static size_t get_mutex(hashmap_s *hashmap, size_t hash) {
	return hash % hashmap->num_locks;
}

static size_t get_bucket_index(hashmap_s *hashmap, void *key) {
	unsigned int hash = hashmap->hash(key);
	return hash % MYCLIB_HASHMAP_SIZE;
}

static void free_bucket_content(hashmap_s *hashmap, bucket_s *bucket) {
	if (bucket == NULL) {
		return;
	}

	/* Free key if free function is provided */
	if (hashmap->free_key != NULL && bucket->key != NULL) {
		hashmap->free_key(bucket->key);
	}

	/* Free value if free function is provided */
	if (hashmap->free_value != NULL && bucket->value != NULL) {
		hashmap->free_value(bucket->value);
	}
}

static bucket_s *find_bucket(hashmap_s *hashmap, void *key, bucket_s **prev) {
	size_t index = get_bucket_index(hashmap, key);
	bucket_s *bucket = &hashmap->map[index];

	*prev = NULL;

	/* Return NULL if first bucket is empty */
	if (bucket->key == NULL) {
		return NULL;
	}

	/* Search through the collision chain */
	while (bucket != NULL) {
		if (hashmap->equal(bucket->key, key)) {
			return bucket;
		}
		*prev = bucket;
		bucket = bucket->next;
	}

	return NULL;
}

hashmap_s *hm_new(hash_f *hash_fn, equal_f *equal_fn, free_key_f *free_key_fn,
				  free_value_f *free_value_fn, size_t key_size, size_t value_size) {
	hashmap_s *hashmap = malloc(sizeof(hashmap_s));
	if (hashmap == NULL) {
		return NULL;
	}

	hashmap->hash = hash_fn;
	hashmap->equal = equal_fn;
	hashmap->free_key = free_key_fn;
	hashmap->free_value = free_value_fn;
	hashmap->key_size = key_size;
	hashmap->value_size = value_size;

	hashmap->size = 0;

	hashmap->num_locks = 64;
	hashmap->locks = malloc(sizeof(mtx_t) * hashmap->num_locks);
	if (hashmap->locks == NULL) {
		free(hashmap);

		return NULL;
	}

	int ret;
	for (size_t i = 0; i < hashmap->num_locks; ++i) {
		ret = mtx_init(&(hashmap->locks[i]), mtx_recursive);
		if (ret != thrd_success) {
			/* Mutex failed */
			for (size_t j = 0; j < i; ++j) {
				mtx_destroy(&(hashmap->locks[j]));
			}

			free(hashmap->locks);
			free(hashmap);
		}
	}

	memset(hashmap->map, 0, sizeof(hashmap->map));

	return hashmap;
}

void hm_free(hashmap_s *hashmap) {
	if (hashmap == NULL) {
		return;
	}

	/* Iterate through all buckets in the hash map */
	for (size_t i = 0; i < MYCLIB_HASHMAP_SIZE; ++i) {
		bucket_s *bucket = &hashmap->map[i];

		/* Free the first bucket if it contains data */
		if (bucket->key != NULL) {
			free_bucket_content(hashmap, bucket);
		}

		/* Free all chained buckets */
		bucket = bucket->next;
		while (bucket != NULL) {
			bucket_s *next = bucket->next;
			free_bucket_content(hashmap, bucket);
			free(bucket);
			bucket = next;
		}
	}

	/* Free the mutex */
	for (size_t i = 0; i < hashmap->num_locks; ++i) {
		mtx_destroy(&(hashmap->locks[i]));
	}
	free(hashmap->locks);

	/* Free the hash map structure itself */
	free(hashmap);
}

void hm_free_bucket(bucket_s *bucket) {
	if (bucket == NULL) {
		return;
	}

	free(bucket->key);
	free(bucket->value);
	free(bucket);
}

bool hm_set(hashmap_s *hashmap, void *key, void *value) {
	if (hashmap == NULL || key == NULL || value == NULL) {
		return false;
	}

	size_t mutex_id = get_mutex(hashmap, hashmap->hash(key));
	mtx_t *mutex = &(hashmap->locks[mutex_id]);
	if (mtx_lock(mutex) != thrd_success) {
		return false;
	}

	bucket_s *prev;
	bucket_s *existing = find_bucket(hashmap, key, &prev);

	if (existing != NULL) {
		/* Key exists, update value */
		if (hashmap->free_value != NULL && existing->value != NULL) {
			hashmap->free_value(existing->value);
		}

		existing->value = malloc(hashmap->value_size);
		if (existing->value == NULL) {
			mtx_unlock(mutex);

			return false;
		}

		memcpy(existing->value, value, hashmap->value_size);

		/* Increase size */
		hashmap->size++;

		mtx_unlock(mutex);

		return true;
	}

	/* Key doesn't exist, need to insert new bucket */
	size_t index = get_bucket_index(hashmap, key);
	bucket_s *bucket = &hashmap->map[index];

	if (bucket->key == NULL) {
		/* First bucket is empty, use it */
		bucket->key = malloc(hashmap->key_size);
		if (bucket->key == NULL) {
			mtx_unlock(mutex);

			return false;
		}

		bucket->value = malloc(hashmap->value_size);
		if (bucket->value == NULL) {
			free(bucket->key);
			bucket->key = NULL;
			mtx_unlock(mutex);

			return false;
		}

		hashmap->size++;
		memcpy(bucket->key, key, hashmap->key_size);
		memcpy(bucket->value, value, hashmap->value_size);
		bucket->next = NULL;
		mtx_unlock(mutex);

		return true;
	}

	/* Create new bucket and insert at head of collision chain */
	bucket_s *new_bucket = malloc(sizeof(bucket_s));
	if (new_bucket == NULL) {
		mtx_unlock(mutex);

		return false;
	}

	new_bucket->key = malloc(hashmap->key_size);
	if (new_bucket->key == NULL) {
		free(new_bucket);
		mtx_unlock(mutex);

		return false;
	}

	new_bucket->value = malloc(hashmap->value_size);
	if (new_bucket->value == NULL) {
		free(new_bucket->key);
		free(new_bucket);
		mtx_unlock(mutex);

		return false;
	}

	hashmap->size++;
	memcpy(new_bucket->key, key, hashmap->key_size);
	memcpy(new_bucket->value, value, hashmap->value_size);
	new_bucket->next = bucket->next;
	bucket->next = new_bucket;

	mtx_unlock(mutex);

	return true;
}

static bucket_s *get_bucket_copy(bucket_s *from, size_t key_size, size_t value_size) {
	bucket_s *copy = malloc(sizeof(bucket_s));
	if (copy == NULL) {
		return NULL;
	}
	memcpy(copy, from, sizeof(bucket_s));

	copy->key = malloc(key_size);
	if (copy->key == NULL) {
		free(copy);

		return NULL;
	}
	memcpy(copy->key, from->key, key_size);

	copy->value = malloc(value_size);
	if (copy->value == NULL) {
		free(copy->key);
		free(copy);

		return NULL;
	}
	memcpy(copy->value, from->value, value_size);

	return copy;
}

bucket_s *hm_get(hashmap_s *hashmap, void *key) {
	if (hashmap == NULL || key == NULL) {
		return NULL;
	}

	size_t mutex_id = get_mutex(hashmap, hashmap->hash(key));
	mtx_t *mutex = &(hashmap->locks[mutex_id]);
	if (mtx_lock(mutex) != thrd_success) {
		return NULL;
	}

	bucket_s *prev;
	bucket_s *found = find_bucket(hashmap, key, &prev);

	if (found) {
		bucket_s *copy = get_bucket_copy(found, hashmap->key_size, hashmap->value_size);

		mtx_unlock(mutex);

		return copy;
	}

	mtx_unlock(mutex);

	return NULL;
}

bool hm_remove(hashmap_s *hashmap, void *key) {
	if (hashmap == NULL || key == NULL) {
		return false;
	}

	size_t mutex_id = get_mutex(hashmap, hashmap->hash(key));
	mtx_t *mutex = &(hashmap->locks[mutex_id]);
	if (mtx_lock(mutex) != thrd_success) {
		return false;
	}

	bucket_s *prev;
	bucket_s *to_remove = find_bucket(hashmap, key, &prev);

	if (to_remove == NULL) {
		mtx_unlock(mutex);

		return false;
	}

	/* Free the content of the bucket */
	free_bucket_content(hashmap, to_remove);

	/* Handle removal based on position in chain */
	if (prev == NULL) {
		/* Removing first bucket in chain */
		if (to_remove->next != NULL) {
			/* Move next bucket's content to first bucket and free the next bucket */
			bucket_s *next_bucket = to_remove->next;
			to_remove->key = next_bucket->key;
			to_remove->value = next_bucket->value;
			to_remove->next = next_bucket->next;
			free(next_bucket);
		} else {
			/* No next bucket, mark first bucket as empty */
			to_remove->key = NULL;
			to_remove->value = NULL;
			to_remove->next = NULL;
		}
	} else {
		/* Removing bucket from middle/end of chain */
		prev->next = to_remove->next;
		free(to_remove);
	}

	hashmap->size--;

	mtx_unlock(mutex);

	return true;
}

size_t hm_size(hashmap_s *hashmap) {
	if (hashmap == NULL) {
		return 0;
	}

	/* Use the first mutex */
	mtx_t *mutex = &hashmap->locks[0];
	if (mtx_lock(mutex) != thrd_success) {
		return 0;
	}

	size_t size = hashmap->size;

	mtx_unlock(mutex);

	return size;
}

bool hm_contains(hashmap_s *hashmap, void *key) {
	if (hashmap == NULL || key == NULL) {
		return false;
	}

	bool res = true;

	size_t mutex_id = get_mutex(hashmap, hashmap->hash(key));
	mtx_t *mutex = &hashmap->locks[mutex_id];
	if (mtx_lock(mutex) != thrd_success) {
		return false;
	}

	bucket_s *bucket = hm_get(hashmap, key);
	if (bucket == NULL) {
		res = false;
	} else {
		hm_free_bucket(bucket);
	}

	mtx_unlock(mutex);

	return res;
}

void hm_foreach(hashmap_s *hashmap, void (*callback)(bucket_s *bucket)) {
	if (hashmap == NULL || callback == NULL) {
		return;
	}

	// TODO
}

void hm_clear(hashmap_s *hashmap) {
	if (hashmap == NULL) {
		return;
	}

	// TODO
}
