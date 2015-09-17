CC=clang++
FLAG=-Wall -std=c++11 -g -O2 -pthread
MAIN=main
TEST=test

.PHONY: all test clean

all: test

logger.o:
	$(CC) $(FLAG) -c logger.cpp

test.o:
	$(CC) $(FLAG) -c test.cpp

test: logger.o test.o
	$(CC) $(FLAG) logger.o test.o -lm -o $(TEST)

clean:
	rm *.o $(TEST)
