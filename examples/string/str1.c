#include <assert.h>
#include <stdio.h>

#include "../../string/mystring.h"

int main(void) {
	mcl_string *str = mcl_string_new("Hello, world!", 512);
	assert(str != NULL);

	printf("%s\nsize: %ld, cap: %ld\n", mcl_string_cstr(str), (long)mcl_string_length(str), (long)mcl_string_capacity(str));
	assert(mcl_string_length(str) == 13);

	int ret = mcl_string_append(str, " How are you?");
	assert(ret == 0);

	printf("After append:\n");
	printf("%s\nsize: %ld, cap: %ld\n", mcl_string_cstr(str), (long)mcl_string_length(str), (long)mcl_string_capacity(str));
	assert(mcl_string_length(str) == 26);

	mcl_string_free(str);

	printf("All tests passed successfully.\n");
	return 0;
}
