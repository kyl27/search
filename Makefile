CC = g++
CFLAGS = -std=c++11 -ggdb -Wall -pthread
LFLAGS = -lpthread -lboost_filesystem -lboost_system


all: main

main: main.cpp
	$(CC) main.cpp -o main $(CFLAGS) $(LFLAGS)

clean:
	rm -f *.o main

tar:
	tar -czvf search.tar.gz *.h *.cpp
