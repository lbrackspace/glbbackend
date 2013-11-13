CF= -O0 -I. -ggdb
all: unsorted_map

main.o: main.cpp
	g++ $(CF) -c main.cpp

unsorted_map: main.o
	g++ $(CF) -o unsorted_map main.o

clean:
	rm *.o unsorted_map
