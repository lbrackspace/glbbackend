CF=-O0 -ggdb -I.

all: server client lookup boosttests

IPRecord.o: IPRecord.hh IPRecord.cc
	g++ $(CF) -c IPRecord.cc

client.o: client.c
	gcc $(CF) -c client.c

GlbContainer.o: GlbContainer.hh GlbContainer.cc
	g++ $(CF) -c GlbContainer.cc

boosttests.o: boosttests.cc
	g++ $(CF) -c boosttests.cc

server.o: GlbContainer.hh IPRecord.hh server.cc
	g++ $(CF) -c server.cc

lookup.o: lookup.cc lookup.hh
	g++ $(CF) -c lookup.cc

server: server.o GlbContainer.o IPRecord.o
	g++ $(CF) -o server server.o GlbContainer.o IPRecord.o -lboost_thread-mt -lboost_system-mt -lpthread

lookup: lookup.o
	g++ $(CF) -o lookup lookup.o -lboost_system-mt -lpthread

boosttests: boosttests.o GlbContainer.o IPRecord.o
	g++ $(CF) -o boosttests boosttests.o GlbContainer.o IPRecord.o -lboost_thread-mt -lboost_system-mt -lboost_unit_test_framework-mt

client: client.o
	gcc $(CF) -o client client.o

clean:
	rm *.o server client lookup boosttests client
