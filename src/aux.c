#include "aux.h"
#include <stdlib.h>

int ascii_to_long(const char *c, long *n) {
  char *num_end;
  long number = strtol(c, &num_end, 10);
  if (n) *n = number;
  return *num_end == '\0';
}
