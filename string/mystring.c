#include "mystring.h"

#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

/* Initialize Thread-Specific Storage */

static tss_t buffer_key;
static once_flag buffer_once = ONCE_FLAG_INIT;

typedef struct {
	char *buf;	/**< Allocated buffer */
	size_t cap; /**< Buffer's capacity */
} tl_buffer_s;

static void buffer_destructor(void *buf) {
	tl_buffer_s *tb = (tl_buffer_s *)buf;
	if (tb == NULL) {
		return;
	}
	free(tb->buf);
	free(tb);
}

static void buffer_key_init(void) { tss_create(&buffer_key, buffer_destructor); }

/* Returns the next power of two of a number */
static size_t next_power_two(size_t len) {
	if (len == 0) return 1;

	size_t p = 1;
	while (p < len) {
		if (p > SIZE_MAX / 2) {
			p = len;
			break;
		}
		p <<= 1;
	}

	return p;
}

string_s *string_new(const char *text, size_t initial_capacity) {
	if (text == NULL) {
		return NULL;
	}

	string_s *str = malloc(sizeof(string_s));
	if (str == NULL) {
		return NULL;
	}

	str->size = strlen(text);

	if (initial_capacity != 0 && initial_capacity < (str->size + 1)) {
		/* Can't allocate with this capacity */
		free(str);

		return NULL;
	}

	size_t capacity = initial_capacity;
	if (capacity == 0) {
		/* Calculate the needed capacity */
		capacity = next_power_two(str->size + 1);
	}

	str->capacity = capacity;

	/* Allocate data (text) buffer */
	str->data = malloc(str->capacity);
	if (str->data == NULL) {
		free(str);

		return NULL;
	}

	/* Copy the text and ensure null termination */
	memcpy(str->data, text, str->size);
	str->data[str->size] = '\0';

	/* Init mutex */
	if (mtx_init(&str->lock, mtx_plain) != thrd_success) {
		free(str->data);
		free(str);

		return NULL;
	}

	return str;
}

int string_append(string_s *string, const char *text) {
	if (string == NULL || text == NULL) {
		return -1;
	}

	if (mtx_lock(&string->lock) != thrd_success) {
		return -1;
	}

	/* Handle empty case */
	size_t text_len = strlen(text);
	if (text_len == 0) {
		mtx_unlock(&string->lock);

		return 0;
	}

	size_t new_size = string->size + text_len;

	/* Check if we need to resize */
	if (new_size + 1 > string->capacity) {
		size_t new_capacity = next_power_two(new_size + 1);
		/* Reallocate the buffer */
		void *new_data = realloc(string->data, new_capacity);
		if (!new_data) {
			mtx_unlock(&string->lock);

			return -1;
		}

		string->data = new_data;
		string->capacity = new_capacity;
	}

	/* Append text */
	memcpy(string->data + string->size, text, text_len);
	string->size = new_size;
	string->data[string->size] = '\0';

	mtx_unlock(&string->lock);

	return 0;
}

int string_extend(string_s *destination, string_s *source) {
	if (destination == NULL || source == NULL) {
		return -1;
	}

	if (mtx_lock(&destination->lock) != thrd_success) {
		return -1;
	}

	if (mtx_lock(&source->lock) != thrd_success) {
		mtx_unlock(&destination->lock);
		return -1;
	}

	size_t need = destination->size + source->size;
	if (need > destination->capacity) {
		/* Reallocate destination data buffer */
		destination->capacity = next_power_two(need);
		char *tmp = realloc(destination->data, destination->capacity);
		if (tmp == NULL) {
			mtx_unlock(&destination->lock);
			mtx_unlock(&source->lock);

			return -1;
		}
		destination->data = tmp;
	}

	/* Copy memory from source data buffer */
	memcpy(destination->data + destination->size, source->data, source->size);
	destination->size = need;
	destination->data[destination->size] = '\0';

	mtx_unlock(&destination->lock);
	mtx_unlock(&source->lock);

	return 0;
}

void string_free(string_s *string) {
	if (string == NULL) {
		return;
	}

	if (string->data) {
		free(string->data);
	}

	mtx_destroy(&string->lock);

	free(string);
}

size_t string_length(string_s *string) {
	if (string == NULL) {
		return 0;
	}

	if (mtx_lock(&string->lock) != thrd_success) {
		return 0;
	}

	size_t len = string->size;

	mtx_unlock(&string->lock);

	return len;
}

size_t string_capacity(string_s *string) {
	if (string == NULL) {
		return 0;
	}

	if (mtx_lock(&string->lock) != thrd_success) {
		return 0;
	}

	size_t cap = string->capacity;

	mtx_unlock(&string->lock);

	return cap;
}

char *string_cstr(string_s *string) {
	if (string == NULL || string->data == NULL) {
		return NULL;
	}

	call_once(&buffer_once, buffer_key_init);

	if (mtx_lock(&string->lock) != thrd_success) {
		return NULL;
	}

	size_t need = string->size + 1;

	/* Retrieve thread local buffer */
	tl_buffer_s *tb = (tl_buffer_s *)tss_get(buffer_key);
	if (tb == NULL) {
		/* Not found, make a new one */
		tb = malloc(sizeof(tl_buffer_s));
		if (tb == NULL) {
			mtx_unlock(&string->lock);

			return NULL;
		}

		tb->cap = next_power_two(need);
		tb->buf = malloc(tb->cap);
		if (tb->buf == NULL) {
			free(tb);
			mtx_unlock(&string->lock);

			return NULL;
		}

		if (tss_set(buffer_key, tb) != thrd_success) {
			free(tb->buf);
			free(tb);
			mtx_unlock(&string->lock);

			return NULL;
		}
	} else if (tb->cap < need) {
		/* Found, but we need a bigger buffer */
		size_t newcap = next_power_two(need);
		char *tmp = realloc(tb->buf, newcap);
		if (tmp == NULL) {
			mtx_unlock(&string->lock);

			return NULL;
		}

		tb->buf = tmp;
		tb->cap = newcap;
	}

	memcpy(tb->buf, string->data, need);

	mtx_unlock(&string->lock);

	return tb->buf;
}

int string_compare(string_s *s1, string_s *s2) {
	if (s1 == NULL || s2 == NULL) {
		return -123;
	}

	if (mtx_lock(&s1->lock) != thrd_success) {
		return -123;
	}

	if (mtx_lock(&s2->lock) != thrd_success) {
		mtx_unlock(&s1->lock);

		return -123;
	}

	int ret = strcmp(s1->data, s2->data);

	mtx_unlock(&s1->lock);
	mtx_unlock(&s2->lock);

	return ret;
}

void string_clear(string_s *string) {
	if (string == NULL) {
		return;
	}

	if (mtx_lock(&string->lock) != thrd_success) {
		return;
	}

	memset(string->data, 0, string->size);
	string->size = 0;

	mtx_unlock(&string->lock);
}

void string_toupper(string_s *string) {
	if (string == NULL) {
		return;
	}

	if (mtx_lock(&string->lock) != thrd_success) {
		return;
	}

	for (size_t i = 0; i < string->size; ++i) {
		string->data[i] = (char)toupper((unsigned char)string->data[i]);
	}

	mtx_unlock(&string->lock);
}

void string_tolower(string_s *string) {
	if (string == NULL) {
		return;
	}

	if (mtx_lock(&string->lock) != thrd_success) {
		return;
	}

	for (size_t i = 0; i < string->size; ++i) {
		string->data[i] = (char)tolower((unsigned char)string->data[i]);
	}

	mtx_unlock(&string->lock);
}

/* Build Longest Prefix Suffix array */
static void build_lsp(int *lps, const char *substring, size_t sub_len) {
	int len = 0;
	int i = 1;

	while (i < sub_len) {
		if (substring[i] == substring[len]) {
			lps[i++] = ++len;
		} else {
			if (len != 0) {
				len = lps[len - 1];
			} else {
				lps[i++] = 0;
			}
		}
	}
}

int string_find(string_s *string, const char *substring) {
	if (string == NULL || substring == NULL) {
		return -1;
	}

	/* Handle empty case */
	if (strcmp(string->data, "") == 0) {
		return -1;
	}

	size_t sub_len = strlen(substring);
	int lps[sub_len];
	memset(lps, 0, sizeof(lps));
	build_lsp(lps, substring, sub_len);

	size_t i = 0; /* string iterator */
	size_t j = 0; /* substring iterator */
	while (i < string->size) {
		if (string->data[i] == substring[j]) {
			i++;
			j++;
			if (j == sub_len) {
				return i - j;
			}
		} else {
			if (j != 0) {
				j = lps[j - 1];
			} else {
				i++;
			}
		}
	}

	return -1;
}
