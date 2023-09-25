#ifndef __OPT_H__
#define __OPT_H__

typedef enum {
  PM_Opts_Invalid,
  PM_Show_Help,
  PM_Eval_File,
  PM_Eval_Expr,
  PM_Eval_Stdin
} ProgramMode;

typedef struct {
  ProgramMode mode;
  int verbose;

  char *expression;
  char *filename;
} Options;

Options opt_get(int argc, char **argv);
int opt_destroy(Options *opt);

int opt_invalid(Options *opt);

#endif
