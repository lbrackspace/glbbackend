#include <string>
#include <iostream>
#include "ServerSingleton.hh"
#include "server.hh"

/* static */ ServerSingleton & ServerSingleton::getInstance(std::string ip, int port){
  std::cout << "Asking for instance..." << std::endl;
  static ServerSingleton instance(ip, port);
  return instance;
}

ServerSingleton::ServerSingleton(std::string ip, int port){
  std::cout << "Instance created" << std::endl;
  start_server_thread(ip, port);
}

ServerSingleton::~ServerSingleton(){
  // TODO Auto-generated destructor stub
}

