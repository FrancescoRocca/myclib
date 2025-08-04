#include <stdio.h>
#include <stdlib.h>

#include "../../string/mystring.h"

int main(void) {
	size_t length;
	size_t capacity;
	char *c_str;

	/* Allocate a new dynamic string with an initial capacity */
	/* Always remember to check return values */
	mcl_string *str = mcl_string_new("Hello, world!", 512);

	/* Retrieve a C str from mcl_string with mcl_string_cstr() */
	c_str = mcl_string_cstr(str);
	length = mcl_string_length(str);
	capacity = mcl_string_capacity(str);
	printf("%s\nlength: %ld, capacity: %ld\n", c_str, length, capacity);
	free(c_str);

	/* Append text to a mcl_string */
	mcl_string_append(str, " How are you?");

	puts("After append:");
	c_str = mcl_string_cstr(str);
	length = mcl_string_length(str);
	capacity = mcl_string_capacity(str);
	printf("%s\nsize: %ld, cap: %ld\n", c_str, length, capacity);
	free(c_str);

	/* Always deallocate memory */
	mcl_string_free(str);

	return 0;
}
