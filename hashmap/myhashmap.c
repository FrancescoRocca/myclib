#include "myhashmap.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * @brief Returns the mutex ID based on hash value.
 */
static inline size_t get_mutex(hashmap_s *hashmap, size_t hash) {
	return hash % hashmap->num_locks;
}

/*
 * @brief Returns the bucket index for a given key.
 */
static inline size_t get_bucket_index(hashmap_s *hashmap, void *key) {
	unsigned int hash = hashmap->hash(key);
	return (size_t)(hash % MYCLIB_HASHMAP_SIZE);
}

/*
 * @brief Free the contents of a bucket (key and value).
 */
static void free_bucket_content(hashmap_s *hashmap, bucket_s *bucket) {
	if (bucket == NULL) {
		return;
	}

	if (bucket->key != NULL) {
		if (hashmap->free_key != NULL) {
			hashmap->free_key(bucket->key);
		} else {
			free(bucket->key);
		}
		bucket->key = NULL;
	}

	if (bucket->value != NULL) {
		if (hashmap->free_value != NULL) {
			hashmap->free_value(bucket->value);
		} else {
			free(bucket->value);
		}
		bucket->value = NULL;
	}
}

/*
 * @brief Find a bucket by key in the chain.
 * @param[out] prev Set to the previous bucket in the chain (or NULL if first).
 */
static bucket_s *find_bucket(hashmap_s *hashmap, void *key, bucket_s **prev) {
	size_t index = get_bucket_index(hashmap, key);
	bucket_s *bucket = &hashmap->map[index];
	*prev = NULL;

	if (bucket->key == NULL) {
		return NULL;
	}

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
	if (hash_fn == NULL || equal_fn == NULL || key_size == 0 || value_size == 0) {
		return NULL;
	}

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

	atomic_init(&hashmap->size, 0);

	hashmap->num_locks = 64;
	hashmap->locks = malloc(sizeof(mtx_t) * hashmap->num_locks);
	if (hashmap->locks == NULL) {
		free(hashmap);
		return NULL;
	}

	for (size_t i = 0; i < hashmap->num_locks; ++i) {
		if (mtx_init(&(hashmap->locks[i]), mtx_plain) != thrd_success) {
			for (size_t j = 0; j < i; ++j) {
				mtx_destroy(&(hashmap->locks[j]));
			}
			free(hashmap->locks);
			free(hashmap);
			return NULL;
		}
	}

	memset(hashmap->map, 0, sizeof(hashmap->map));

	return hashmap;
}

void hm_free(hashmap_s *hashmap) {
	if (hashmap == NULL) {
		return;
	}

	for (size_t i = 0; i < MYCLIB_HASHMAP_SIZE; ++i) {
		bucket_s *bucket = &hashmap->map[i];

		if (bucket->key != NULL || bucket->value != NULL) {
			free_bucket_content(hashmap, bucket);
		}

		bucket = bucket->next;
		while (bucket != NULL) {
			bucket_s *next = bucket->next;
			free_bucket_content(hashmap, bucket);
			free(bucket);
			bucket = next;
		}
	}

	for (size_t i = 0; i < hashmap->num_locks; ++i) {
		mtx_destroy(&(hashmap->locks[i]));
	}
	free(hashmap->locks);

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

	unsigned int hash = hashmap->hash(key);
	size_t mutex_id = get_mutex(hashmap, hash);
	mtx_t *mutex = &(hashmap->locks[mutex_id]);

	if (mtx_lock(mutex) != thrd_success) {
		return false;
	}

	bucket_s *prev = NULL;
	bucket_s *existing = find_bucket(hashmap, key, &prev);

	if (existing != NULL) {
		/* Key exists - update value */
		void *new_value = malloc(hashmap->value_size);
		if (new_value == NULL) {
			mtx_unlock(mutex);
			return false;
		}
		memcpy(new_value, value, hashmap->value_size);

		/* Free old value and assign new one */
		if (hashmap->free_value != NULL && existing->value != NULL) {
			hashmap->free_value(existing->value);
		} else if (existing->value != NULL) {
			free(existing->value);
		}
		existing->value = new_value;

		mtx_unlock(mutex);
		return true;
	}

	/* Key doesn't exist - insert new bucket */
	size_t index = get_bucket_index(hashmap, key);
	bucket_s *bucket = &hashmap->map[index];

	if (bucket->key == NULL) {
		/* Primary bucket is empty */
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

		memcpy(bucket->key, key, hashmap->key_size);
		memcpy(bucket->value, value, hashmap->value_size);
		bucket->next = NULL;

		atomic_fetch_add(&hashmap->size, 1);
		mtx_unlock(mutex);
		return true;
	}

	/* Collision - create new bucket and chain it */
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

	memcpy(new_bucket->key, key, hashmap->key_size);
	memcpy(new_bucket->value, value, hashmap->value_size);

	/* Insert at head of chain */
	new_bucket->next = bucket->next;
	bucket->next = new_bucket;

	atomic_fetch_add(&hashmap->size, 1);
	mtx_unlock(mutex);
	return true;
}

/*
 * @brief Create a copy of a bucket.
 */
static bucket_s *get_bucket_copy(bucket_s *from, size_t key_size, size_t value_size) {
	if (from == NULL) {
		return NULL;
	}

	bucket_s *copy = malloc(sizeof(bucket_s));
	if (copy == NULL) {
		return NULL;
	}
	copy->key = NULL;
	copy->value = NULL;
	copy->next = NULL;

	if (from->key != NULL) {
		copy->key = malloc(key_size);
		if (copy->key == NULL) {
			free(copy);
			return NULL;
		}
		memcpy(copy->key, from->key, key_size);
	}

	if (from->value != NULL) {
		copy->value = malloc(value_size);
		if (copy->value == NULL) {
			free(copy->key);
			free(copy);
			return NULL;
		}
		memcpy(copy->value, from->value, value_size);
	}

	return copy;
}

bucket_s *hm_get(hashmap_s *hashmap, void *key) {
	if (hashmap == NULL || key == NULL) {
		return NULL;
	}

	unsigned int hash = hashmap->hash(key);
	size_t mutex_id = get_mutex(hashmap, hash);
	mtx_t *mutex = &(hashmap->locks[mutex_id]);

	if (mtx_lock(mutex) != thrd_success) {
		return NULL;
	}

	bucket_s *prev = NULL;
	bucket_s *found = find_bucket(hashmap, key, &prev);

	bucket_s *copy = NULL;
	if (found != NULL) {
		copy = get_bucket_copy(found, hashmap->key_size, hashmap->value_size);
	}

	mtx_unlock(mutex);
	return copy;
}

bool hm_remove(hashmap_s *hashmap, void *key) {
	if (hashmap == NULL || key == NULL) {
		return false;
	}

	unsigned int hash = hashmap->hash(key);
	size_t mutex_id = get_mutex(hashmap, hash);
	mtx_t *mutex = &(hashmap->locks[mutex_id]);

	if (mtx_lock(mutex) != thrd_success) {
		return false;
	}

	bucket_s *prev = NULL;
	bucket_s *to_remove = find_bucket(hashmap, key, &prev);

	if (to_remove == NULL) {
		mtx_unlock(mutex);
		return false;
	}

	if (prev == NULL) {
		/* Removing primary bucket */
		if (to_remove->next != NULL) {
			/* Move next bucket content to primary */
			bucket_s *next_bucket = to_remove->next;

			free_bucket_content(hashmap, to_remove);

			to_remove->key = next_bucket->key;
			to_remove->value = next_bucket->value;
			to_remove->next = next_bucket->next;

			free(next_bucket);
		} else {
			/* No chain, just clear */
			free_bucket_content(hashmap, to_remove);
			to_remove->next = NULL;
		}
	} else {
		/* Removing from chain */
		prev->next = to_remove->next;
		free_bucket_content(hashmap, to_remove);
		free(to_remove);
	}

	atomic_fetch_sub(&hashmap->size, 1);

	mtx_unlock(mutex);
	return true;
}

size_t hm_size(hashmap_s *hashmap) {
	if (hashmap == NULL) {
		return 0;
	}

	return atomic_load(&hashmap->size);
}

bool hm_contains(hashmap_s *hashmap, void *key) {
	if (hashmap == NULL || key == NULL) {
		return false;
	}

	bucket_s *b = hm_get(hashmap, key);
	if (b != NULL) {
		hm_free_bucket(b);
		return true;
	}

	return false;
}

void hm_foreach(hashmap_s *hashmap, void (*callback)(bucket_s *bucket)) {
	if (hashmap == NULL || callback == NULL) {
		return;
	}

	/* Lock all mutexes */
	for (size_t i = 0; i < hashmap->num_locks; ++i) {
		mtx_lock(&hashmap->locks[i]);
	}

	for (size_t i = 0; i < MYCLIB_HASHMAP_SIZE; ++i) {
		bucket_s *bucket = &hashmap->map[i];

		if (bucket->key != NULL) {
			bucket_s *copy = get_bucket_copy(bucket, hashmap->key_size, hashmap->value_size);
			if (copy != NULL) {
				callback(copy);
				hm_free_bucket(copy);
			}
		}

		bucket = bucket->next;
		while (bucket != NULL) {
			bucket_s *copy = get_bucket_copy(bucket, hashmap->key_size, hashmap->value_size);
			if (copy != NULL) {
				callback(copy);
				hm_free_bucket(copy);
			}
			bucket = bucket->next;
		}
	}

	/* Unlock all mutexes */
	for (size_t i = 0; i < hashmap->num_locks; ++i) {
		mtx_unlock(&hashmap->locks[i]);
	}
}

void hm_clear(hashmap_s *hashmap) {
	if (hashmap == NULL) {
		return;
	}

	/* Lock all mutexes */
	for (size_t i = 0; i < hashmap->num_locks; ++i) {
		mtx_lock(&hashmap->locks[i]);
	}

	for (size_t i = 0; i < MYCLIB_HASHMAP_SIZE; ++i) {
		bucket_s *bucket = &hashmap->map[i];

		if (bucket->key != NULL || bucket->value != NULL) {
			free_bucket_content(hashmap, bucket);
		}

		bucket = bucket->next;
		while (bucket != NULL) {
			bucket_s *next = bucket->next;
			free_bucket_content(hashmap, bucket);
			free(bucket);
			bucket = next;
		}

		hashmap->map[i].key = NULL;
		hashmap->map[i].value = NULL;
		hashmap->map[i].next = NULL;
	}

	atomic_store(&hashmap->size, 0);

	/* Unlock all mutexes */
	for (size_t i = 0; i < hashmap->num_locks; ++i) {
		mtx_unlock(&hashmap->locks[i]);
	}
}
