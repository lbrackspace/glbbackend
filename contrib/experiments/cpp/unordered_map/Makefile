CF= -O0 -I. -ggdb
all: unordered_map

main.o: main.cpp
	g++ $(CF) -c main.cpp

unordered_map: main.o
	g++ $(CF) -o unordered_map main.o

clean:
	rm *.o unordered_map
