#ifndef __AUX_H__
#define __AUX_H__

#include <stdio.h>

#define log(...) fprintf(stderr, __VA_ARGS__)

int ascii_to_long(const char *c, long *n);
FILE *file_from_string(const char *c);
int char_is_whitespace(char c);

#endif
