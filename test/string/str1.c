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
	string_s *str = string_new("Hello, world!", 512);
	assert(str != NULL);

	/* Retrieve a C str from string with mcl_string_cstr() */
	c_str = string_cstr(str);
	length = string_len(str);
	capacity = string_cap(str);
	assert(strcmp(c_str, "Hello, world!") == 0);
	assert(length == 13);
	assert(capacity == 512);

	/* Append text to a mcl_string */
	assert(string_append(str, " How are you?") == 0);

	c_str = string_cstr(str);
	length = string_len(str);
	capacity = string_cap(str);
	assert(strcmp(c_str, "Hello, world! How are you?") == 0);
	assert(length == 26);
	assert(capacity == 512);

	/* Always deallocate memory */
	string_free(str);
}
