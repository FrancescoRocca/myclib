/**
 * Ignore this file
 */

#include <assert.h>
#include <stdio.h>

int main(void) {
	puts("==== [Running Hashmap tests] ====");
	test_hm1();
	puts("OK!");
	puts("");

	puts("==== [Running Queue tests] ====");
	test_q1();
	puts("OK!");
	puts("");

	puts("==== [Running String tests] ====");
	test_str1();
	test_str2();
	puts("OK!");
	puts("");

	return 0;
}
