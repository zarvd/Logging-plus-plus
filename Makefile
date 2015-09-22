CC=clang++
FLAG=-Wall -std=c++11 -g -O2 -pthread
SRCS = $(wildcard *.cpp)
OBJS=$(SRCS:.cpp=.o)
OBJS_WITH_NO_TEST = $(filter-out test.o, $(OBJS))
TEST=test

.PHONY: all test clean

all: test

$(OBJS):
	$(CC) $(FLAG) -c $(addsuffix .cpp, $(basename $@))

prod: $(OBJS)
	mkdir -p ./prod
	ld -r $(OBJS_WITH_NO_TEST) -o ./prod/Logger.o

test: $(OBJS)
	$(CC) $(FLAG) $(OBJS) -lm -o $(TEST)

clean:
	rm *.o $(TEST)
