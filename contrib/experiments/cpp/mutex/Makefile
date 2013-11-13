CF=-O0 -ggdb

all: mutex

mutex.o: mutex.cpp mutex.h
	g++ $(CF) -c mutex.cpp

mutex: mutex.o
	g++ $(CF) -o mutex mutex.o -lboost_thread-mt -lboost_system-mt -lpthread

clean:
	rm *.o mutex
