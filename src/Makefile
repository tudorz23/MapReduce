CC = g++
CFLAGS = -Wall -Wextra

TARGETS = tema1

all: $(TARGETS)

tema1: main.cpp
	$(CC) $(CFLAGS) main.cpp -o tema1 -lpthread

build:
	g++ main.cpp -o tema1 -lpthread

run:
	./tema1

clean:
	rm -f *.o $(TARGETS)