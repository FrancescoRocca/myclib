#include "mystring.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Initialize Thread-Specific Data Keys */
static tss_t buffer_key;
static once_flag buffer_once = ONCE_FLAG_INIT;

typedef struct {
	char *buf;
	size_t cap;
} tl_buffer_t;

static void buffer_destructor(void *buf) {
	tl_buffer_t *tb = (tl_buffer_t *)buf;
	if (tb == NULL) {
		return;
	}
	free(tb->buf);
	free(tb);
}

static void buffer_key_init(void) { tss_create(&buffer_key, buffer_destructor); }

/* Dynamic String library */
size_t mcl_next_power_two(size_t len) {
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

mcl_string_s *mcl_string_new(const char *text, size_t initial_capacity) {
	if (text == NULL) {
		return NULL;
	}

	mcl_string_s *str = malloc(sizeof(mcl_string_s));
	if (str == NULL) {
		return NULL;
	}

	str->size = strlen(text);

	if (initial_capacity != 0 && initial_capacity < (str->size + 1)) {
		free(str);

		return NULL;
	}

	size_t capacity = initial_capacity;
	if (capacity == 0) {
		capacity = mcl_next_power_two(str->size + 1);
	}

	str->capacity = capacity;

	/* Allocate data buffer */
	str->data = malloc(str->capacity);
	if (str->data == NULL) {
		free(str);

		return NULL;
	}

	/* Copy the text and ensure null termination */
	memcpy(str->data, text, str->size);
	str->data[str->size] = '\0';

	/* Init pthread mutex */
	if (mtx_init(&str->lock, mtx_plain) != thrd_success) {
		free(str->data);
		free(str);

		return NULL;
	}

	return str;
}

int mcl_string_append(mcl_string_s *string, const char *text) {
	if (string == NULL || text == NULL) {
		return -1;
	}

	/* Lock resource */
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
		size_t new_capacity = mcl_next_power_two(new_size + 1);
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

void mcl_string_free(mcl_string_s *string) {
	if (string == NULL) {
		return;
	}

	if (string->data) {
		free(string->data);
	}

	mtx_destroy(&string->lock);

	free(string);
}

size_t mcl_string_length(mcl_string_s *string) {
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

size_t mcl_string_capacity(mcl_string_s *string) {
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

char *mcl_string_cstr(mcl_string_s *string) {
	if (string == NULL || string->data == NULL) {
		return NULL;
	}

	call_once(&buffer_once, buffer_key_init);

	if (mtx_lock(&string->lock) != thrd_success) {
		return NULL;
	}

	size_t need = string->size + 1;

	tl_buffer_t *tb = (tl_buffer_t *)tss_get(buffer_key);
	if (tb == NULL) {
		tb = malloc(sizeof(*tb));
		if (tb == NULL) {
			mtx_unlock(&string->lock);

			return NULL;
		}

		tb->cap = mcl_next_power_two(need);
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
		size_t newcap = mcl_next_power_two(need);
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
