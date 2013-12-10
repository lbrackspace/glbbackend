#ifndef STARTUPCLIENT_H
#define	STARTUPCLIENT_H

#include <string>

class StartUpClient {
private:
    std::string host;
    int port;
public:

    StartUpClient() {
    }

    StartUpClient(std::string _host, int _port) : host(_host), port(_port) {
    }

    StartUpClient(const StartUpClient& orig) {
    }

    virtual ~StartUpClient() {
    }

    void setPort(int _port) {
        port = _port;
    }

    void setHost(std::string _host) {
        host = _host;
    }

    bool connect();


};

#endif	/* STARTUPCLIENT_H */

