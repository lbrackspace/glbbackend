#include <string>
#include <iostream>
#include "ServerSingleton.hh"
#include "GLBCommandServer.hh"

ServerSingleton & ServerSingleton::getInstance(std::string ip, int port) {
    static ServerSingleton instance(ip, port);
    return instance;
}

ServerSingleton::ServerSingleton(std::string ip, int port) {
    GLBCommandServer server;
    server.start(ip, port);
}

ServerSingleton::~ServerSingleton() {
    // TODO Auto-generated destructor stub
}

