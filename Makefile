CC = clang++
CFLAGS = -std=c++11 -stdlib=libc++ -ggdb -Wall -pthread
LFLAGS =


all: main

main: main.cpp
	$(CC) $(CFLAGS) $(LFLAGS) -o main main.cpp

clean:
	rm -f *.o main

tar:
	tar -czvf search.tar.gz *.h *.cpp
