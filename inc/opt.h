#ifndef __OPT_H__
#define __OPT_H__

#include <stdio.h>

typedef struct {
  FILE *input;
  int be_verbose;

  char *_input_buffer;
} Options;

Options opt_get(int argc, char **argv);
int opt_destroy(Options *opt);

#endif
