#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  int b;
  int e;
  int E;
  int n;
  int s;
  int t;
  int T;
  int v;
} flags;

void print_nonprintable(unsigned char c) {
  if (c == '\n' || c == '\t') {
    putchar(c);
  } else if (c < 32) {
    printf("^%c", c + 64);
  } else if (c == 127) {
    printf("^?");
  } else {
    putchar(c);
  }
}

int should_process_char(char c, int fl_s, int *last_was_newline,
                        int *empty_lines) {
  int result = 1;

  if (fl_s) {
    if (c == '\n') {
      if (*last_was_newline) {
        (*empty_lines)++;
        if (*empty_lines > 1) {
          result = 0;
        }
      } else {
        *empty_lines = 0;
      }
      *last_was_newline = 1;
    } else {
      *last_was_newline = 0;
      *empty_lines = 0;
    }
  }

  return result;
}

void process_file(FILE *f, flags fl, int *global_line_num) {
  int c;
  int line_num = 0;
  int empty_lines = 0;
  int new_line = 1;
  int last_was_newline = 0;

  if (fl.n || fl.b) {
    line_num = *global_line_num;
  }

  while ((c = fgetc(f)) != EOF) {
    int process_char =
        should_process_char(c, fl.s, &last_was_newline, &empty_lines);

    if (process_char) {
      if (new_line) {
        if (fl.b && c != '\n') {
          printf("%6d\t", ++line_num);
        } else if (fl.n && !fl.b) {
          printf("%6d\t", ++line_num);
        }
        new_line = 0;
      }

      if ((fl.t || fl.T) && c == '\t') {
        printf("^I");
      } else if ((fl.e || fl.E) && c == '\n') {
        if (fl.e || fl.E) {
          printf("$");
        }
        putchar(c);
        new_line = 1;
      } else if (fl.v && c != '\n' && c != '\t') {
        if (c < 32) {
          printf("^%c", c + 64);
        } else if (c == 127) {
          printf("^?");
        } else {
          putchar(c);
        }
      } else {
        putchar(c);
        if (c == '\n') {
          new_line = 1;
        }
      }
    }
  }

  if (fl.n || fl.b) {
    *global_line_num = line_num;
  }
}

int main(int argc, char **argv) {
  flags fl = {0, 0, 0, 0, 0, 0, 0, 0};
  int opt;
  int option_index = 0;
  int global_line_num = 0;

  struct option long_opts[] = {{"number-nonblank", no_argument, 0, 'b'},
                               {"number", no_argument, 0, 'n'},
                               {"squeeze-blank", no_argument, 0, 's'},
                               {0, 0, 0, 0}};

  while ((opt = getopt_long(argc, argv, "beEnstTv", long_opts,
                            &option_index)) != -1) {
    switch (opt) {
      case 'b':
        fl.b = 1;
        break;
      case 'e':
        fl.e = 1;
        fl.v = 1;
        break;
      case 'E':
        fl.E = 1;
        break;
      case 'n':
        fl.n = 1;
        break;
      case 's':
        fl.s = 1;
        break;
      case 't':
        fl.t = 1;
        fl.v = 1;
        break;
      case 'T':
        fl.T = 1;
        break;
      case 'v':
        fl.v = 1;
        break;
    }
  }

  if (fl.b && fl.n) {
    fl.n = 0;
  }

  if (optind >= argc) {
    process_file(stdin, fl, &global_line_num);
  } else {
    for (int i = optind; i < argc; i++) {
      FILE *f = fopen(argv[i], "r");
      if (f) {
        process_file(f, fl, &global_line_num);
        fclose(f);
      } else {
        fprintf(stderr, "cat: %s: %s\n", argv[i], strerror(errno));
      }
    }
  }

  return 0;
}