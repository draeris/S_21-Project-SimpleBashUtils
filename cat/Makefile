CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c11
TARGET = s21_cat
TEST_SCRIPT = ../tests/autotest_cat.sh

all: $(TARGET)

$(TARGET): s21_cat.c
	$(CC) $(CFLAGS) s21_cat.c -o $(TARGET)

test: $(TARGET)
	@bash $(TEST_SCRIPT) || true

clean:
	rm -f $(TARGET)

re: clean all

.PHONY: all test clean re