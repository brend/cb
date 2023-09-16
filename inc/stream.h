#ifndef __STREAM_H__
#define __STREAM_H__

#include <stdio.h>

typedef struct {
	FILE *file;
	char buffer[64];
	int buffer_length;
} stream;

stream stream_open_file(const char *filename);

int stream_is_open(stream *s);

int stream_close(stream *s);

int stream_consume_whitespace(stream *s);

int stream_consume_char(stream *s, char *c);

int stream_has_prefix(stream *s, const char *prefix);

int stream_consume(stream *s, const char *prefix);

int stream_consume_alphanum_prefix(stream *s, char *buffer, int buffer_size);

void stream_dump(stream *s);

#endif