CF= -O0 -I. -ggdb

all: cppdemo boosttests

main.o: main.cpp DemoUtils.h Matrix.h ThreadManager.h Point.h ring_buffer.h
	g++ $(CF) -c main.cpp 

ring_buffer.o: ring_buffer.cpp ring_buffer.h
	g++ $(CF) -c ring_buffer.cpp

Point.o: Point.cpp Point.h
	g++ $(CF) -c Point.cpp

BoostTests.o: BoostTests.cpp
	g++ $(CF) -c BoostTests.cpp

ThreadManager.o: ThreadManager.cpp ThreadManager.h
	g++ $(CF) -c ThreadManager.cpp

DemoUtils.o: DemoUtils.cpp DemoUtils.h
	g++ $(CF) -c DemoUtils.cpp

Matrix.o: Matrix.h Matrix.cpp
	g++ $(CF) -c Matrix.cpp

cppdemo: DemoUtils.o Matrix.o ThreadManager.o Point.o ring_buffer.o main.o
	g++ $(CF) -o cppdemo main.o DemoUtils.o Matrix.o ThreadManager.o Point.o ring_buffer.o -lboost_thread-mt -lboost_system-mt

boosttests: BoostTests.o DemoUtils.o Matrix.o ThreadManager.o Point.o
	g++ $(CF) -o boosttests BoostTests.o DemoUtils.o Matrix.o ThreadManager.o Point.o -lboost_thread-mt -lboost_system-mt -lboost_unit_test_framework-mt
clean:
	rm *.o cppdemo boosttests
