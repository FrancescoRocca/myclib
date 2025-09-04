#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../hashmap/myhashmap.h"

#define MAX_STR_LEN 64

/* My custom data type stored as value */
struct my_custom_type {
	int age;
	char favourite_brand[MAX_STR_LEN];
};

/* Let's hash our name */
static unsigned int my_hash_func(const void *key) {
	char *name = (char *)key;
	size_t len = strlen(name);

	unsigned int hash = 0;
	for (size_t i = 0; i < len; ++i) {
		hash += (int)name[i];
	}

	return hash % 2069;
}

/* Let's write our compare function */
bool my_equal_fun(const void *key_a, const void *key_b) {
	char *name_a = (char *)key_a;
	char *name_b = (char *)key_b;

	if (strcmp(name_a, name_b) == 0) {
		return true;
	}

	return false;
}

/* And our last two functions, the free key and value called inside mcl_hm_remove() */
void my_free_key(void *key) { free(key); }

void my_free_value(void *value) {
	struct my_custom_type *mct = (struct my_custom_type *)value;
	free(mct);
}

int main(void) {
	/* Allocate a new hashmap */
	/* Pass your custom hash, equal and free functions */
	/* This hashmap will contain names as keys and a custom type as value */
	size_t key_size = sizeof(char) * MAX_STR_LEN;
	size_t value_size = sizeof(int) + sizeof(char) * MAX_STR_LEN;
	mcl_hashmap_s *map = mcl_hm_init(my_hash_func, my_equal_fun, my_free_key, my_free_value, key_size, value_size);

	/* Set a new value */
	struct my_custom_type p1 = {
		.age = 21,
	};
	strncpy(p1.favourite_brand, "Ferrari", sizeof(p1.favourite_brand));

	mcl_hm_set(map, "John", &p1);

	/* Retrieve the data */
	mcl_bucket_s *john = mcl_hm_get(map, "John");
	char *name = (char *)john->key;
	struct my_custom_type *john_v = (struct my_custom_type *)john->value;
	int age = john_v->age;
	char *fav_brand = john_v->favourite_brand;
	fprintf(stdout, "Name: %s\nAge: %d\nFavourite brand: %s\n", name, age, fav_brand);
	mcl_hm_free_bucket(john);

	/* Remove a key from hash map */
	mcl_hm_remove(map, "John");

	/* Deallocate */
	mcl_hm_free(map);

	return 0;
}
