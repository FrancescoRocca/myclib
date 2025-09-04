#include <stdio.h>
#include <stdlib.h>

#include "../../string/mystring.h"

int main(void) {
	size_t length;
	size_t capacity;
	char *c_str;

	/* Allocate a new dynamic string with an initial capacity */
	mcl_string_s *str = mcl_string_new("Hello, world!", 512);
	if (str == NULL) {
		printf("Failed to initialize string");
		exit(EXIT_FAILURE);
	}

	/* Retrieve a C str from mcl_string with mcl_string_cstr() */
	c_str = mcl_string_cstr(str);
	length = mcl_string_length(str);
	capacity = mcl_string_capacity(str);
	printf("%s\nlength: %lld, capacity: %lld\n", c_str, length, capacity);

	/* Append text to a mcl_string */
	mcl_string_append(str, " How are you?");

	puts("After append:");
	c_str = mcl_string_cstr(str);
	length = mcl_string_length(str);
	capacity = mcl_string_capacity(str);
	printf("%s\nsize: %lld, cap: %lld\n", c_str, length, capacity);

	/* Always deallocate memory */
	mcl_string_free(str);

	return 0;
}
