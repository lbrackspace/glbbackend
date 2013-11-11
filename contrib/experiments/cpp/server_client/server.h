#ifndef __SERVER_H
#define __SERVER_H

#include<string>
#include<boost/asio.hpp>
#include<boost/shared_ptr.hpp>

int usage(char *prog);
int listener(std::string ip_addr_str, int port);
int server(boost::shared_ptr<boost::asio::ip::tcp::socket> socket);
#endif


