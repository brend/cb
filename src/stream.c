#include "stream.h"
#include <string.h>
#include <stdlib.h>

Stream *stream_open_file(const char *filename) {
	Stream *input = malloc(sizeof(Stream));

	memset(input, 0, sizeof(Stream));
	input->file = fopen(filename, "r");
	input->buffer = queue_new(1024);
	
	return input;
}

int stream_is_open(Stream *s) {
	return s != NULL && s->file != NULL;
}

int stream_close(Stream *s) {
	if (s != NULL && s->file != NULL) {
		fclose(s->file);
		s->file = NULL;
		free(s->buffer);
		return 1;
	} else {
		return 0;
	}
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
	if (queue_is_empty(s->buffer) && stream_is_open(s)) {
		int d = fgetc(s->file);
		
		if (d != EOF) {
			queue_enqueue_char(s->buffer, (char)d);
		}
	}

	if (queue_is_empty(s->buffer)) { return 0; }

	char x = queue_dequeue_char(s->buffer);

	if (c) { *c = x; }

	stream_update_position(s, x);
	
	return 1;
}

int stream_consume(Stream *s, const char *prefix) {
	if (stream_has_prefix(s, prefix)) {
		for (int i = 0; i < strlen(prefix); i++) {
			stream_consume_char(s, NULL);
		}
		return 1;
	} else {
		return 0;
	}
}

int stream_consume_whitespace(Stream *s) {
	while (
		stream_has_prefix(s, " ")  || 
		stream_has_prefix(s, "\t") ||
		stream_has_prefix(s, "\r") ||
		stream_has_prefix(s, "\n"))
	{
		stream_consume_char(s, NULL);
	}
	return 1;
}

int stream_consume_alphanum_prefix(Stream *s, char *buffer, int buffer_size) {
	char c;
	int i = 0;
	
	while (i < buffer_size - 1 && 
		!stream_has_prefix(s, " ") &&
		!stream_has_prefix(s, "\t") &&
		!stream_has_prefix(s, "\r") &&
		!stream_has_prefix(s, "\n") && // TODO: Handle non-alphanum with peek
		stream_consume_char(s, &c))
	{
		buffer[i] = c;
		i++;
	}
	
	buffer[i] = 0;
	
	return i;
}

int buffer_has_prefix(Queue *s, const char *prefix, int len) {
	if (len == 0) { return 1; }
	if (len > queue_size(s)) { return 0; }

	const char *comp = prefix;

	for (int i = 0; i < len; i++) {
		char c = 0;

		if (!(queue_peeki_char(s, i, &c) && c == *comp++)) {
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
	
	int c;
	int i = len;

	while (i-- > 0 && (c = fgetc(s->file)) != EOF) {
		queue_enqueue_char(s->buffer, c);
	}
	
	return buffer_has_prefix(s->buffer, prefix, len);
}
