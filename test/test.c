/**
 * Ignore this file
 */

#include <assert.h>
#include <stdio.h>

void test_hm1(void);

void test_queue1(void);

void test_str1(void);
void test_str2(void);
void test_str3(void);

void test_vec1(void);

void test_stack1(void);

void test_socket1();

int main(void) {
	puts("==== [Running Hashmap tests] ====");
	test_hm1();
	puts("OK!");
	puts("");

	puts("==== [Running Queue tests] ====");
	test_queue1();
	puts("OK!");
	puts("");

	puts("==== [Running String tests] ====");
	test_str1();
	test_str2();
	test_str3();
	puts("OK!");
	puts("");

	puts("==== [Running Vector tests] ====");
	test_vec1();
	puts("OK!");
	puts("");

	puts("==== [Running Stack tests] ====");
	test_stack1();
	puts("OK!");
	puts("");

	puts("==== [Running Socket tests] ====");
	test_socket1();
	puts("OK!");
	puts("");

	return 0;
}
