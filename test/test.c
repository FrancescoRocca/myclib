/**
 * Ignore this file
 */

#include <assert.h>
#include <stdio.h>

void test_hm1();
void test_q1();
void test_str1();
void test_str2();

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
