#include "mystring.h"

#include <stdlib.h>
#include <string.h>

static size_t mcl_next_power_two(size_t len) {
	len--;
	len |= len >> 1;
	len |= len >> 2;
	len |= len >> 4;
	len |= len >> 8;
	len |= len >> 16;
	len++;

	return len;
}

mcl_string *mcl_string_new(const char *text, size_t initial_capacity) {
	if (text == NULL) {
		return NULL;
	}

	/* Allocate string struct */
	mcl_string *str = malloc(sizeof(mcl_string));
	if (str == NULL) {
		return NULL;
	}

	/* Calculate size and capacity */
	str->size = strlen(text);
	size_t capacity = initial_capacity;

	if (capacity != 0 && capacity - 1 < str->size) {
		free(str);

		return NULL;
	}

	if (capacity == 0) {
		capacity = mcl_next_power_two(str->size + 1);
	}

	str->capacity = capacity;

	/* Allocate data buffer */
	str->data = malloc(sizeof(char) * str->capacity);
	if (str->data == NULL) {
		free(str);

		return NULL;
	}

	/* Copy the text and ensure null termination */
	memcpy(str->data, text, str->size);
	str->data[str->size] = '\0';

	/* Init pthread mutex */
	int ret = pthread_mutex_init(&str->lock, NULL);
	if (ret != 0) {
		free(str->data);
		free(str);

		return NULL;
	}

	return str;
}

int mcl_string_append(mcl_string *string, const char *text) {
	if (string == NULL || text == NULL) {
		return -1;
	}

	/* Lock resource */
	int ret = pthread_mutex_lock(&string->lock);
	if (ret != 0) {
		return -1;
	}

	/* Handle empty case */
	size_t text_len = strlen(text);
	if (text_len == 0) {
		pthread_mutex_unlock(&string->lock);

		return 0;
	}

	size_t new_size = text_len + string->size;

	/* Check if we need to resize */
	if (new_size + 1 > string->capacity) {
		size_t new_capacity = mcl_next_power_two(new_size + 1);
		/* Reallocate the buffer */
		void *new_data = realloc(string->data, sizeof(char) * new_capacity);
		if (!new_data) {
			pthread_mutex_unlock(&string->lock);

			return -1;
		}

		string->data = new_data;
		string->capacity = new_capacity;

		/* Init to 0 the new capacity */
		memset(string->data + string->size, 0, string->capacity - string->size);
	}

	/* Append text */
	memcpy(string->data + string->size, text, text_len);
	string->size = new_size;
	string->data[string->size] = '\0';

	/* Unlock resource */
	ret = pthread_mutex_unlock(&string->lock);
	if (ret != 0) {
		return -1;
	}

	return 0;
}

void mcl_string_free(mcl_string *string) {
	if (string == NULL) {
		return;
	}

	if (string->data) {
		free(string->data);
	}

	pthread_mutex_destroy(&string->lock);

	free(string);
}

size_t mcl_string_length(mcl_string *string) {
	if (string == NULL) {
		return 0;
	}

	int ret = pthread_mutex_lock(&string->lock);
	if (ret != 0) {
		return 0;
	}

	size_t len = string->size;

	pthread_mutex_unlock(&string->lock);

	return len;
}

size_t mcl_string_capacity(mcl_string *string) {
	if (string == NULL) {
		return 0;
	}

	int ret = pthread_mutex_lock(&string->lock);
	if (ret != 0) {
		return 0;
	}

	size_t cap = string->capacity;

	pthread_mutex_unlock(&string->lock);

	return cap;
}

char *mcl_string_cstr(mcl_string *string) {
	if (string == NULL || string->data == NULL) {
		return NULL;
	}

	int ret = pthread_mutex_lock(&string->lock);
	if (ret != 0) {
		return NULL;
	}

	char *data = malloc(sizeof(char) * string->size + 1);
	if (data == NULL) {
		pthread_mutex_unlock(&string->lock);

		return NULL;
	}

	memcpy(data, string->data, string->size + 1);

	pthread_mutex_unlock(&string->lock);

	return data;
}
