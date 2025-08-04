#include <stdio.h>
#include <stdlib.h>

#include "../../string/mystring.h"

int main(void) {
	mcl_string *str = mcl_string_new("Hello, world!", 0);
	mcl_string_append(str, " How are you?");

	char *c_str = mcl_string_cstr(str);
	size_t len = mcl_string_length(str);
	size_t cap = mcl_string_capacity(str);

	fprintf(stdout, "%s\nlen: %ld\ncapacity: %ld\n", c_str, len, cap);

	mcl_string_free(str);
	free(c_str);

	return 0;
}
