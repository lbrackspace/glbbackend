#include <string>
#include <iostream>
#include "ServerSingleton.hh"
#include "GLBCommandServer.hh"

/* static */ ServerSingleton & ServerSingleton::getInstance(std::string ip, int port){
  static ServerSingleton instance(ip, port);
  return instance;
}

ServerSingleton::ServerSingleton(std::string ip, int port){
  GLBCommandServer server(ip, port);
  server.start();
}

ServerSingleton::~ServerSingleton(){
  // TODO Auto-generated destructor stub
}

