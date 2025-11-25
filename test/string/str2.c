#include "../string/mystring.h"
#include <assert.h>
#include <string.h>

void test_str2(void) {
	string_s *s1 = string_new("Hello, world!", 0);
	assert(s1 != NULL);
	string_s *s2 = string_new("Hello, world!", 0);
	assert(s2 != NULL);
	/* Don't call mcl_string_cstr() more than once in the same printf function */

	int ret = string_compare(s1, s2);
	assert(ret == 0);

	string_clear(s1);
	ret = string_compare(s1, s2);
	assert(ret != 0);

	string_tolower(s1);
	string_toupper(s2);
	assert(strcmp(string_cstr(s1), "") == 0);
	assert(strcmp(string_cstr(s2), "HELLO, WORLD!") == 0);

	/* Extend a string */
	string_s *extend_me = string_new("This string is suuuuuuuuuuuuuuuuuuuuuper extended!", 0);
	string_extend(s1, extend_me);
	assert(string_len(s1) == 50);
	assert(string_cap(s1) == 64);

	/* Find substring in string */
	int pos = string_find(s1, " is ");
	assert(pos == 11);

	string_free(s1);
	string_free(s2);
	string_free(extend_me);
}
