CXX = clang++
FLAG = -Wall -std=c++11 -g -O2 -pthread
SRCS = $(wildcard src/*.cpp)
OBJS = $(SRCS:.cpp=.o)
OBJS_WITH_NO_TEST = $(filter-out test.o, $(OBJS))
TEST = test

.PHONY: all test clean

all: test

$(OBJS):
	$(CXX) $(FLAG) -c $(addsuffix .cpp, $(basename $@)) -o $@

staticLib: $(OBJS)
	ar rcs libLogger.a $(OBJS)

prod: $(OBJS)
	mkdir -p ./prod
	ld -r $(OBJS_WITH_NO_TEST) -o ./prod/Logger.o

test: $(OBJS)
	$(CXX) $(FLAG) $(OBJS) -lm -o $(TEST)

clean:
	-rm *.o $(TEST)
	-rm -r prod
