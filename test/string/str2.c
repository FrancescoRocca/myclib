#include <assert.h>
#include <string.h>

#include "../../string/mystring.h"

void test_str2(void) {
	mcl_string_s *s1 = mcl_string_new("Hello, world!", 0);
	assert(s1 != NULL);
	mcl_string_s *s2 = mcl_string_new("Hello, world!", 0);
	assert(s2 != NULL);
	/* Don't call mcl_string_cstr() more than once in the same printf function */

	int ret = mcl_string_compare(s1, s2);
	assert(ret == 0);

	mcl_string_clear(s1);
	ret = mcl_string_compare(s1, s2);
	assert(ret != 0);

	mcl_string_tolower(s1);
	mcl_string_toupper(s2);
	assert(strcmp(mcl_string_cstr(s1), "") == 0);
	assert(strcmp(mcl_string_cstr(s2), "HELLO, WORLD!") == 0);

	mcl_string_free(s1);
	mcl_string_free(s2);
}
