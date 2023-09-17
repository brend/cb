#include "stream.h"
#include <string.h>
#include <stdlib.h>

stream stream_open_file(const char *filename) {
	stream input;
	
	memset(&input, 0, sizeof(input));
	
	input.file = fopen(filename, "r");
	
	return input;
}

int stream_is_open(stream *s) {
	return s != NULL && s->file != NULL;
}

int stream_close(stream *s) {
	if (s != NULL && s->file != NULL) {
		fclose(s->file);
		s->file = NULL;
		return 1;
	} else {
		return 0;
	}
}

void stream_update_position(stream *s, char c) {
	if (c == '\n') {
		s->line++;
		s->column = 0;
	} else {
		s->column++;
	}
}

int stream_consume_char(stream *s, char *c) {
	if (s->buffer_length > 0) {
		char x = s->buffer[0];

		if (c) { *c = x; }

		stream_update_position(s, x);
		
		int p = 1;

		while (p < sizeof(s->buffer)) {
			s->buffer[p-1] = s->buffer[p];
			p++;
		}
		
		s->buffer_length -= 1;
		
		return 1;
	}
	
	if (!stream_is_open(s)) { return 0; }
	
	int d = fgetc(s->file);
	
	if (c) {
		*c = d;
	}

	if (d != EOF) {
		stream_update_position(s, (char)d);
	}
	
	return d != EOF;
}

void stream_dump(stream *s) {
	printf("{file=%p, buffer=%s, buffer_length=%d}\n", s->file, s->buffer, s->buffer_length);
}

int stream_consume(stream *s, const char *prefix) {
	if (stream_has_prefix(s, prefix)) {
		size_t len = strlen(prefix);
		int p = len;
		int i = 0;
		
		s->buffer[i++] = s->buffer[p++];
		s->buffer[i] = 0;
		s->buffer_length = strlen(s->buffer);

		for (int j = 0; j < len; ++j) {
			stream_update_position(s, prefix[j]);
		}
		
		return i;
	} else {
		return 0;
	}
}

int stream_consume_whitespace(stream *s) {
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

int stream_consume_alphanum_prefix(stream *s, char *buffer, int buffer_size) {
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

int stream_has_prefix(stream *s, const char *prefix) {
	if (!(s && s->file && prefix)) { return 0; }
	
	size_t len = strlen(prefix);
	
	if (len == 0) { return 0; }
	
	if (len >= sizeof(s->buffer)) {
		printf("buffer overrun\n");
		exit(2);
	}
	
	int i = s->buffer_length;
	char c;
	
	while (i < len && (c = fgetc(s->file)) != EOF) {
		s->buffer[i++] = c;
	}
	
	s->buffer[i] = 0;
	s->buffer_length = i;
	
	if (strncmp(prefix, s->buffer, len) == 0) {
		return i;
	}
	
	return 0;
}
