CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c11
CAT_EXE = cat/s21_cat
GREP_EXE = grep/s21_grep

all: s21_cat s21_grep

s21_cat: cat/s21_cat.c
	$(CC) $(CFLAGS) cat/s21_cat.c -o $(CAT_EXE)

s21_grep: grep/s21_grep.c
	$(CC) $(CFLAGS) grep/s21_grep.c -o $(GREP_EXE)

test: test_cat test_grep

test_cat: all
	@bash tests/autotest_cat.sh || true

test_grep: all
	@bash tests/autotest_grep.sh || true

clean:
	rm -f $(CAT_EXE) $(GREP_EXE) tests/*.log

re: clean all

.PHONY: all s21_cat s21_grep test test_cat test_grep clean re