#!/bin/bash

export CF="-O0 -ggdb -I."

rm *o
rm server
rm boosttests
echo "Building SOAContainer.cc"
g++ ${CF} -c SOAContainer.cc
echo "Building GLBCollection.cc"
g++ ${CF} -c GLBCollection.cc
echo "Building IPRecord.cc"
g++ ${CF} -c IPRecord.cc
echo "Building GlbContainer.cc"
g++ ${CF} -c GlbContainer.cc
echo "Building server.cc"
g++ ${CF} -c server.cc
echo "Building server_main.cc"
g++ ${CF} -c server_main.cc
echo "Building boosttests.cc"
g++ ${CF} -c boosttests.cc
echo "Linking server"
g++ ${CF} -o server server_main.o server.o GLBCollection.o GlbContainer.o IPRecord.o SOAContainer.o -lboost_thread-mt -lboost_system-mt -lpthread
echo "Linking boosttests"
g++ ${CF} -o boosttests boosttests.o GlbContainer.o IPRecord.o SOAContainer.o -lboost_thread-mt -lboost_system-mt -lboost_unit_test_framework-mt

