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
      options.expression = malloc(strlen(argv[i]) + 1);
      strcpy(options.expression, argv[i]);
      use_expression = 0;
      continue;
    }

    if (EQ("-e")) {
      options.mode = PM_Eval_Expr;
      use_expression = 1;
        continue;
    } else if (EQ("-h") || EQ("--help")) {
      options.mode = PM_Show_Help;
      break;
    } else if (EQ("-v") || EQ("--verbose")) {
      options.verbose = 1;
      continue;
    }

    // argument without switch must be a file name
    options.mode = PM_Eval_File;
    options.filename = malloc(strlen(argv[i]));
    strcpy(options.filename, argv[i]);
  }

  // if no input has been specified, read from stdin
  if (options.mode == PM_Opts_Invalid) {
    options.mode = PM_Eval_Stdin;
  }

  return options;
}

int opt_destroy(Options *opt) {
  if (!opt) { return 0; }

  if (opt->filename) {
    free(opt->filename);
    opt->filename = NULL;
  }

  if (opt->expression) {
    free(opt->expression);
    opt->expression = NULL;
  }

  return 1;
}

int opt_invalid(Options *opt) {
  return opt->mode == PM_Opts_Invalid;
}
