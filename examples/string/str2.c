#include <stdio.h>

#include "../../string/mystring.h"

int main(void) {
	mcl_string_s *s1 = mcl_string_new("Hello, world!", 0);
	mcl_string_s *s2 = mcl_string_new("Hello, world!", 0);

	/* Dont' call mcl_string_cstr() more than once in the same printf function */
	printf("s1: %s\n", mcl_string_cstr(s1));
	printf("s2: %s\n", mcl_string_cstr(s2));

	int ret = mcl_string_compare(s1, s2);
	if (ret == 0) {
		printf("Same string!\n");
	}

	mcl_string_clear(s1);
	if (mcl_string_compare(s1, s2) != 0) {
		printf("Not the same!\n");
	}

	mcl_string_toupper(s1);
	mcl_string_tolower(s2);

	printf("s1: %s\n", mcl_string_cstr(s1));
	printf("s2: %s\n", mcl_string_cstr(s2));

	mcl_string_free(s1);
	mcl_string_free(s2);
}
