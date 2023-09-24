#include "opt.h"
#include <string.h>
#include <stdlib.h>
#include "aux.h"

#define EQ(s) (strcmp((s), argv[i]) == 0)

Options opt_get(int argc, char **argv) {
  Options options = {0};
  int use_expression = 0;

  for (int i = 1; i < argc; ++i) {
    if (use_expression) {
      options.input = file_from_string(argv[i]);
      use_expression = 0;
      continue;
    }

    if (EQ("-e")) {
      use_expression = 1;
        continue;
    }

    // argument without switch must be a file name
    if (!options.input) {
      options.input = fopen(argv[i], "r");
    } else {
      fprintf(stderr, "filename \"%s\" will be ignored: input already specified\n", argv[i]);
    }
    break;
  }

  // if no input has been specified, read from stdin
  if (!options.input) {
    options._input_buffer = read_stdin_to_end();
    options.input = file_from_string(options._input_buffer);
  }

  return options;
}

int opt_destroy(Options *opt) {
  if (!opt) { return 0; }

  if (opt->input) {
    fclose(opt->input);
    opt->input = NULL;
  }

  if (opt->_input_buffer) {
    free(opt->_input_buffer);
    opt->_input_buffer = NULL;
  }

  return 1;
}
