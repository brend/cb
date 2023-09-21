#ifndef __STREAM_H__
#define __STREAM_H__

#include <stdio.h>
#include "queue.h"

typedef struct {
	FILE *file;
	int line;
	int column;
	Queue *buffer;
} Stream;

Stream *stream_open_file(const char *filename);

Stream *stream_from_file_ptr(FILE *file);

Stream *stream_from_string(const char *string);

int stream_is_open(Stream *s);

int stream_close(Stream *s);

int stream_consume_whitespace(Stream *s);

int stream_consume_char(Stream *s, char *c);

int stream_has_prefix(Stream *s, const char *prefix);

int stream_consume(Stream *s, const char *prefix);

int stream_consume_alphanum_prefix(Stream *s, char *buffer, int buffer_size);

#endif