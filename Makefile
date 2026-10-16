CC      ?= gcc
CFLAGS  ?= -std=c11 -Wall -Wextra -Wpedantic -Werror -O2
CFLAGS  += -D_DEFAULT_SOURCE
SAN      = -fsanitize=address,undefined -fno-omit-frame-pointer -g

LIB_SRC = src/game.c src/words.c src/scores.c

.PHONY: all test sanitize clean

all: hangman

hangman: src/main.c $(LIB_SRC) src/*.h
	$(CC) $(CFLAGS) -o $@ src/main.c $(LIB_SRC)

test_runner: tests/test_hangman.c $(LIB_SRC) src/*.h
	$(CC) $(CFLAGS) -o $@ tests/test_hangman.c $(LIB_SRC)

test: test_runner
	./test_runner

# Rebuild the tests with AddressSanitizer and UndefinedBehaviorSanitizer to catch
# buffer overflows, use after free, and other memory errors
sanitize: tests/test_hangman.c $(LIB_SRC)
	$(CC) $(CFLAGS) $(SAN) -o test_runner_san tests/test_hangman.c $(LIB_SRC)
	./test_runner_san

clean:
	rm -f hangman test_runner test_runner_san *.tmp
