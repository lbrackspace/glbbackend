#include <string>
#include <boost/thread.hpp>


#ifndef SERVERSINGLETON_HH_
#define SERVERSINGLETON_HH_

class ServerSingleton
{
public:
  static ServerSingleton & getInstance(std::string ip, int port);
  ~ServerSingleton();
private:
  ServerSingleton(std::string ip, int port);
  ServerSingleton(const ServerSingleton &);
  ServerSingleton & operator = (const ServerSingleton &);
};

extern boost::mutex serverSingletonMutex;

#endif /* SERVERSINGLETON_HH_ */
