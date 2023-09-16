#ifndef __STREAM_H__
#define __STREAM_H__

#include <stdio.h>

typedef struct {
	FILE *file;
} stream;

stream stream_open_file(const char *filename);

int stream_is_open(stream s);

int stream_close(stream s);

int stream_get_char(stream s, char *c);

#endif