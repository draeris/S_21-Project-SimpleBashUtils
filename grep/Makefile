CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c11
TARGET = s21_grep
TEST_SCRIPT = ../tests/autotest_grep.sh

all: $(TARGET)

$(TARGET): s21_grep.c
	$(CC) $(CFLAGS) s21_grep.c -o $(TARGET)

test: $(TARGET)
	@bash $(TEST_SCRIPT) || true

clean:
	rm -f $(TARGET)

re: clean all

.PHONY: all test clean re