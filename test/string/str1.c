#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../string/mystring.h"

void test_str1(void) {
	size_t length;
	size_t capacity;
	char *c_str;

	/* Allocate a new dynamic string with an initial capacity */
	mcl_string_s *str = mcl_string_new("Hello, world!", 512);
	assert(str != NULL);

	/* Retrieve a C str from string with mcl_string_cstr() */
	c_str = mcl_string_cstr(str);
	length = mcl_string_length(str);
	capacity = mcl_string_capacity(str);
	assert(strcmp(c_str, "Hello, world!") == 0);
	assert(length == 13);
	assert(capacity == 512);

	/* Append text to a mcl_string */
	assert(mcl_string_append(str, " How are you?") == 0);

	c_str = mcl_string_cstr(str);
	length = mcl_string_length(str);
	capacity = mcl_string_capacity(str);
	assert(strcmp(c_str, "Hello, world! How are you?") == 0);
	assert(length == 26);
	assert(capacity == 512);

	/* Always deallocate memory */
	mcl_string_free(str);
}
