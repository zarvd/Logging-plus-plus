CC=clang++
FLAG=-Wall -std=c++11 -g -O2 -pthread
SRCS = $(wildcard *.cpp)
OBJS=$(SRCS:.cpp=.o)
MAIN=main
TEST=test

.PHONY: all test clean

all: test

$(OBJS):
	$(CC) $(FLAG) -c $(addsuffix .cpp, $(basename $@))

test: $(OBJS)
	$(CC) $(FLAG) $(OBJS) -lm -o $(TEST)

clean:
	rm *.o $(TEST)
