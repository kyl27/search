CC = g++
CFLAGS = -std=c++11 -ggdb -Wall -pthread
LFLAGS = -lboost_system -lboost_filesystem -lpthread


all: main

main: main.cpp
	$(CC) $(CFLAGS) $(LFLAGS) -o main main.cpp

clean:
	rm -f *.o main

tar:
	tar -czvf search.tar.gz *.h *.cpp
