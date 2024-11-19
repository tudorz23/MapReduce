CC = g++
CFLAGS = -Wall -Wextra -g

TARGETS = tema1

build: $(TARGETS)

all: $(TARGETS)

mapper.o: Mapper.cpp
	$(CC) -c $(CFLAGS) Mapper.cpp -o mapper.o -lpthread

reducer.o: Reducer.cpp
	$(CC) -c $(CFLAGS) Reducer.cpp -o reducer.o -lpthread

main.o: main.cpp
	$(CC) -c $(CFLAGS) main.cpp -o main.o -lpthread

tema1: mapper.o reducer.o main.o
	$(CC) $(CFLAGS) mapper.o reducer.o main.o -o tema1 -lpthread

run:
	./tema1

clean:
	rm -f *.o $(TARGETS)

.PHONY: all build clean pack
