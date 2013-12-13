#include <string>
#include <iostream>
#include <boost/thread.hpp>
#include <boost/thread/locks.hpp>

#include "ServerSingleton.hh"
#include "GLBCommandServer.hh"
#include "GlbContainer.hh"

ServerSingleton & ServerSingleton::getInstance(std::string ip, int port) {
    using namespace std;
    using namespace boost;
    lock_guard<mutex> lock(serverSingletonMutex);
    static ServerSingleton instance(ip, port);
    return instance;
}

ServerSingleton::ServerSingleton(std::string ip, int port) {
    using namespace std;
    using namespace boost;
    if (DEBUG) {
        boost::lock_guard<mutex> lock(debugMutex);
        cout << "Spinning up singleton GLBCommandServer at ip " << ip << " port " << port << endl;
    }
    static GLBCommandServer server(ip, port);
    server.start();
    if (DEBUG) {
        boost::lock_guard<mutex> lock(debugMutex);
        cout << "finished spinning up singleton GLBCommadnServer" << endl;
    }
}

ServerSingleton::~ServerSingleton() {
    using namespace std;
    using namespace boost;
    if (DEBUG) {
        boost::lock_guard<mutex> lock(debugMutex);
        cout << "ServerSingleton destructor called" << endl;
    }
}

boost::mutex serverSingletonMutex;
