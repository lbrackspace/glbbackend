#include <string>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <boost/thread.hpp>
#include "GLBCommandServer.hh"
#include "StartUpClient.hh"

int usage(char *prog) {
    using namespace std;
    cout << "usage is " << prog << " <ip> <port>" << endl;
    cout << endl;
    cout << "Spawns off a threaded servicer that echos back commands" << endl;
    return 0;
}

int main(int argc, char **argv) {
    using namespace std;
    if (argc < 3) {
        usage(argv[0]);
        return -1;
    }
    bool useClient = false;
    cout << "pid = " << getpid() << endl;
    string ip_addr_str(argv[1]);
    int port = std::atoi(argv[2]);
    if (argc > 3 && string(argv[3]).compare("client") == 0) {
        useClient = true;
    }
    if (useClient) {
        StartUpClient sc(ip_addr_str, port);
        bool status = sc.connect();
        cout << "startup client reported: " << boolalpha << status << endl;
    } else {
        GLBCommandServer server;
        server.start(ip_addr_str, port);
    }
    return 0;
}

