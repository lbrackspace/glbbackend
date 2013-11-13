CF= -O0 -I. -ggdb

all: stdin_loop

main.o: main.cpp
	g++ $(CF) -c main.cpp

stdin_loop: main.o
	g++ $(CF) -o stdin_loop main.o

clean:
	rm *.o stdin_loop
