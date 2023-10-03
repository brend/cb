#include "stream.h"
#include <string.h>
#include <stdlib.h>
#include "aux.h"

Stream *stream_new(FILE *file) {
	Stream *s = malloc(sizeof(Stream));

	memset(s, 0, sizeof(Stream));
	s->buffer = queue_new(1024);
	s->file = file;
	
	return s;
}

Stream *stream_open_file(const char *filename) {
	return stream_new(fopen(filename, "r"));
}

Stream *stream_from_file(FILE *file) {
	return stream_new(file);
}

Stream *stream_from_string(const char *string) {
	return stream_new(file_from_string(string));
}

int stream_is_open(Stream *s) {
	return s != NULL && s->file != NULL;
}

int stream_destroy(Stream **s) {
	if (!(s && *s)) { return 0; }

/*
	if ((*s)->file != NULL) {
		if (!fclose((*s)->file)) { return 0; }
		(*s)->file = NULL;
	}
*/

	if ((*s)->buffer != NULL) {
		if (!queue_destroy(&(*s)->buffer)) { return 0; }
		(*s)->buffer = NULL;
	}

	free(*s);
	*s = NULL;
	return 1;
}

void stream_update_position(Stream *s, char c) {
	if (c == '\n') {
		s->line++;
		s->column = 0;
	} else {
		s->column++;
	}
}

int stream_consume_char(Stream *s, char *c) {
  	stream_has_prefix(s, " ");

	if (queue_is_empty(s->buffer)) { return 0; }

	char x = queue_dequeue_char(s->buffer);

	if (c) { *c = x; }

	stream_update_position(s, x);
	
	return 1;
}

int stream_consume(Stream *s, const char *prefix) {
	if (!(s && prefix && stream_has_prefix(s, prefix))) { 
		return 0; 
	}

	for (int i = 0; i < strlen(prefix); i++) {
		stream_consume_char(s, NULL);
	}

	return 1;
}

int stream_consume_whitespace(Stream *s) {
	if (!s) { return 0; }

    char c = 0;
    
	while (stream_peek_char(s, &c) && char_is_whitespace(c))
	{
		stream_consume_char(s, NULL);
	}
	
	return 1;
}

int stream_peek_char(Stream *s, char *c) {
	if (!s) { return 0; }

	stream_has_prefix(s, " ");
	if (queue_is_empty(s->buffer)) { return 0; }
	if (c) { queue_peeki_char(s->buffer, 0, c); }
	
	return 1;
}

int stream_consume_alphanum_prefix(Stream *s, char *buffer, int buffer_size) {
	if (!(s && buffer && buffer_size > 0)) { return 0; }

	char c = 0;
	int i = 0;
	int begins_with_number = 0;
	
	while (i < buffer_size - 1 && 
	    stream_peek_char(s, &c) &&
    	((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) &&
    	(!begins_with_number || (c >= '0' && c <= '9')) &&
		stream_consume_char(s, &c))
	{
		buffer[i] = c;
    	if (i == 0) {
			begins_with_number = (c >= '0' && c <= '9');
		}
		i++;
	}
	
	buffer[i] = 0;
	//TODO: Error if id is too long
	return i;
}

int buffer_has_prefix(Queue *q, const char *prefix, size_t len) {
	if (!q) { return 0; }
	if (len == 0) { return 1; }
	if (!prefix) { return 0; }
	if (len > queue_size(q)) { return 0; }

	const char *comp = prefix;

	for (int i = 0; i < len; i++) {
		char c = 0;

		if (!(queue_peeki_char(q, i, &c) && c == *comp++)) {
			return 0;
		}
	}

	return 1;
}

int stream_has_prefix(Stream *s, const char *prefix) {
	if (!(s && s->file && prefix)) { return 0; }
	
	size_t len = strlen(prefix);
	
	if (len == 0) { return 1; }
	
	if (len >= s->buffer->capacity) {
		printf("buffer overrun\n");
		exit(2);
	}
	
	int c = 0;
	size_t i = len;

	while (i-- > 0 && (c = fgetc(s->file)) != EOF) {
		queue_enqueue_char(s->buffer, c);
	}
	
	return buffer_has_prefix(s->buffer, prefix, len);
}
