#include <errno.h>
#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  int v, c, l, n, i, e, multiple_files;
} flags;

char *copy_string(const char *src) {
  char *dest = NULL;
  if (src) {
    size_t len = strlen(src) + 1;
    dest = malloc(len);
    if (dest) {
      for (size_t i = 0; i < len; i++) {
        dest[i] = src[i];
      }
    }
  }
  return dest;
}

int read_line(char **line, size_t *len, FILE *f) {
  int c;
  size_t pos = 0;
  int result = -1;
  int reading = 1;

  if (*line == NULL) {
    *len = 256;
    *line = malloc(*len);
    if (*line == NULL) return -1;
  }

  while (reading) {
    c = fgetc(f);
    if (c == EOF) {
      if (pos > 0) result = (int)pos;
      reading = 0;
    } else {
      if (pos + 1 >= *len) {
        size_t new_len = *len * 2;
        char *new_line = realloc(*line, new_len);
        if (new_line == NULL) {
          result = -1;
          reading = 0;
        } else {
          *line = new_line;
          *len = new_len;
        }
      }

      if (reading) {
        (*line)[pos++] = (char)c;
        if (c == '\n') {
          result = (int)pos;
          reading = 0;
        }
      }
    }
  }

  if (result > 0) (*line)[result] = '\0';
  return result;
}

char *collect_patterns(int argc, char **argv, int *optind, int e_flag) {
  char *pattern = NULL;
  char temp[4096] = {0};
  int first = 1;

  if (e_flag == 0 && *optind < argc) {
    pattern = copy_string(argv[*optind]);
    (*optind)++;
  } else {
    for (int i = 1; i < argc; i++) {
      if (i > 0 && strcmp(argv[i - 1], "-e") == 0) {
        if (first) {
          strcpy(temp, argv[i]);
          first = 0;
        } else {
          strcat(temp, "|(");
          strcat(temp, argv[i]);
          strcat(temp, ")");
        }
      }
    }
    if (temp[0] != '\0') pattern = copy_string(temp);
  }
  return pattern;
}

int count_matches(FILE *f, regex_t *re, flags fl, int *match_found) {
  char *line = NULL;
  size_t len = 0;
  int num = 0;
  int count = 0;
  int continue_reading = 1;
  *match_found = 0;

  while (continue_reading && read_line(&line, &len, f) != -1) {
    num++;
    if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';

    int is_match = (regexec(re, line, 0, NULL, 0) == 0);
    if (fl.v) is_match = !is_match;

    if (is_match) {
      count++;
      *match_found = 1;
      if (fl.l) continue_reading = 0;
    }
  }
  free(line);
  return count;
}

void print_matches(FILE *f, regex_t *re, flags fl, const char *name,
                   int show_name) {
  char *line = NULL;
  size_t len = 0;
  int num = 0;

  rewind(f);
  while (read_line(&line, &len, f) != -1) {
    num++;
    if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';

    int is_match = (regexec(re, line, 0, NULL, 0) == 0);
    if (fl.v) is_match = !is_match;

    if (is_match) {
      if (show_name) printf("%s:", name);
      if (fl.n) printf("%d:", num);
      printf("%s\n", line);
    }
  }
  free(line);
}

int process_file(FILE *f, regex_t *re, flags fl, const char *name,
                 int show_name) {
  int match_found;
  int count = count_matches(f, re, fl, &match_found);

  if (fl.l && match_found) {
    printf("%s\n", name);
  } else if (fl.c) {
    if (show_name) printf("%s:", name);
    printf("%d\n", count);
  } else if (match_found && !fl.c) {
    print_matches(f, re, fl, name, show_name);
  }

  return match_found;
}

int main(int argc, char **argv) {
  flags fl = {0};
  regex_t regex;
  char *pattern = NULL;
  int opt;
  int any_match = 0;
  int exit_status = 0;
  int e_flag = 0;
  int error = 0;

  while (!error && (opt = getopt(argc, argv, "e:ivcln")) != -1) {
    switch (opt) {
      case 'e':
        fl.e = 1;
        e_flag = 1;
        break;
      case 'i':
        fl.i = 1;
        break;
      case 'v':
        fl.v = 1;
        break;
      case 'c':
        fl.c = 1;
        break;
      case 'l':
        fl.l = 1;
        break;
      case 'n':
        fl.n = 1;
        break;
      default:
        fprintf(stderr, "usage: grep [-e pattern] [-ivcln] [file...]\n");
        error = 1;
    }
  }

  if (!error) {
    pattern = collect_patterns(argc, argv, &optind, e_flag);
    if (!pattern) {
      fprintf(stderr, "grep: отсутствует шаблон\n");
      error = 1;
    }
  }

  if (!error) {
    int cflags = REG_EXTENDED | (fl.i ? REG_ICASE : 0);
    if (regcomp(&regex, pattern, cflags) != 0) {
      fprintf(stderr, "grep: ошибка в регулярном выражении\n");
      error = 1;
    }
  }

  if (!error) {
    fl.multiple_files = (argc - optind > 1);

    if (optind >= argc) {
      any_match = process_file(stdin, &regex, fl, "(stdin)", 0);
    } else {
      for (int i = optind; i < argc; i++) {
        FILE *f = fopen(argv[i], "r");
        if (f) {
          if (process_file(f, &regex, fl, argv[i], fl.multiple_files))
            any_match = 1;
          fclose(f);
        } else {
          fprintf(stderr, "grep: %s: %s\n", argv[i], strerror(errno));
          exit_status = 1;
        }
      }
    }
    regfree(&regex);
  }

  if (pattern) free(pattern);
  return (error || (!any_match && exit_status == 0)) ? 1 : 0;
}