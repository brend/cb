#ifndef __AUX_H__
#define __AUX_H__

#define log(...) fprintf(stderr, __VA_ARGS__)

int ascii_to_long(const char *c, long *n);

#endif
