#include "aux.h"
#include <stdlib.h>
#include <string.h>

int ascii_to_long(const char *c, long *n) {
  char *num_end;
  long number = strtol(c, &num_end, 10);
  if (n) *n = number;
  return *num_end == '\0';
}

FILE *file_from_string(const char *string) {
  return fmemopen((void*)string, strlen(string), "r");
}

int char_is_whitespace(char c) {
  switch (c) {
    case ' ':
    case '\t':
    case '\r':
    case '\n':
    return 1;
    default:
    return 0;
  }
}

char *read_stdin_to_end(void) {
  const size_t CHUNK_SIZE = 1024;
  char *buffer = NULL;
  size_t buffer_size = 0;
  size_t bytes_read = 0;
  size_t total_bytes_read = 0;

  do {
    buffer_size += CHUNK_SIZE;
    buffer = realloc(buffer, buffer_size);
    bytes_read = fread(buffer + total_bytes_read, 1, CHUNK_SIZE, stdin);
    total_bytes_read += bytes_read;
  } while (bytes_read == CHUNK_SIZE);

  return buffer;
}
