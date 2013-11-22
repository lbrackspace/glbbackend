CF= -O0 -I. -ggdb
all: unordered_map

main.o: main.cpp
	g++ $(CF) -c main.cpp

unordered_map: main.o
	g++ $(CF) -o unordered_map main.o -lboost_thread-mt -lboost_system-mt

clean:
	rm *.o unordered_map
