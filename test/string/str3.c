#include "../string/mystring.h"
#include <assert.h>
#include <string.h>

int main(void) {
	/* Make a new string from format */
	string_s *s = string_format("My name is %s (%d)", "John", 21);
	assert(strcmp(s->data, "My name is John (21)") == 0);

	/* Test insert */
	string_s *ms = string_new("Heo mate!", 0);
	assert(ms != NULL);
	string_insert(ms, 2, "ll");
	assert(strcmp(ms->data, "Hello mate!") == 0);

	/* Test delete */
	string_remove(ms, 5, 5);
	assert(strcmp(ms->data, "Hello!") == 0);
	assert(string_remove(ms, string_len(ms), 1) == -1);

	string_s *remove_mid = string_new("abcdef", 0);
	assert(remove_mid != NULL);
	assert(string_remove(remove_mid, 2, 2) == 0);
	assert(strcmp(remove_mid->data, "abef") == 0);
	assert(string_remove(remove_mid, 2, 2) == 0);
	assert(strcmp(remove_mid->data, "ab") == 0);
	assert(string_remove(remove_mid, 0, 2) == 0);
	assert(strcmp(remove_mid->data, "") == 0);

	/* Test replace */
	string_s *replace_me = string_new("My car doesn't bark!", 0);
	assert(strcmp(replace_me->data, "My car doesn't bark!") == 0);
	string_replace(replace_me, "car", "dog");
	assert(strcmp(replace_me->data, "My dog doesn't bark!") == 0);

	string_free(ms);
	string_free(s);
	string_free(remove_mid);
	string_free(replace_me);
}
