CF=-O0 -ggdb -I.

all: server client lookup boosttests

IPRecord.o: IPRecord.h IPRecord.cpp
	g++ $(CF) -c IPRecord.cpp

GlbContainer.o: GlbContainer.h GlbContainer.cpp
	g++ $(CF) -c GlbContainer.cpp

boosttests.o: boosttests.cpp
	g++ $(CF) -c boosttests.cpp

ring_buffer.o: ring_buffer.h ring_buffer.cpp
	g++ $(CF) -c ring_buffer.cpp

server.o: ring_buffer.h GlbContainer.h server.cpp
	g++ $(CF) -c server.cpp

client.o: client.cpp
	g++ $(CF) -c client.cpp

lookup.o: lookup.cpp lookup.h
	g++ $(CF) -c lookup.cpp

server: server.o ring_buffer.o GlbContainer.o IPRecord.o
	g++ $(CF) -o server server.o ring_buffer.o GlbContainer.o IPRecord.o -lboost_thread-mt -lboost_system-mt -lpthread

client: client.o
	g++ $(CF) -o client client.o -lboost_thread-mt -lboost_system-mt -lpthread

lookup: lookup.o
	g++ $(CF) -o lookup lookup.o -lboost_system-mt -lpthread

boosttests: boosttests.o ring_buffer.o GlbContainer.o IPRecord.o
	g++ $(CF) -o boosttests boosttests.o ring_buffer.o GlbContainer.o IPRecord.o -lboost_thread-mt -lboost_system-mt -lboost_unit_test_framework-mt

clean:
	rm *.o server client lookup boosttests
