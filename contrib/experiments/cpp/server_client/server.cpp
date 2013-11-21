#include<iostream>
#include<cstdlib>
#include<unistd.h>
#include<deque>
#include<boost/asio.hpp>
#include<boost/thread.hpp>
#include<boost/bind.hpp>
#include<boost/chrono.hpp>
#include"server.h"
#include"GlbContainer.h"
#include "ring_buffer.h"

using namespace std;
using namespace boost;
using namespace boost::asio;

const int SOCKBUFFSIZE = 4096;
const boost::chrono::microseconds sdelay(50);
const int INIT_RING_SIZE = 2;

int main(int argc, char **argv) {
    if (argc < 3) {
        usage(argv[0]);
        return -1;
    }
    string ip_addr_str(argv[1]);
    int port = std::atoi(argv[2]);
    listener(ip_addr_str, port);
    return 0;
}

int usage(char *prog) {
    cout << "usage is " << prog << " <ip> <port>" << endl;
    cout << endl;
    cout << "Spawns off a threaded servicer that echos back commands" << endl;
    return 0;
}

int listener(string ip_addr_str, int port) {
    io_service ios;
    cout << "Resolving ip Address " << ip_addr_str << "for port " << port << endl;
    ip::tcp::endpoint ep(ip::address::from_string(ip_addr_str), port);
    cout << "Resolved endpoint to address: " << ep.address() << endl;
    cout << "Listening on addr: " << ep.address() << " port " << port << endl;
    ip::tcp::acceptor ac(ios, ep);

    while (true) {
        shared_ptr<ip::tcp::iostream> stream(new ip::tcp::iostream());
        ac.accept(*stream->rdbuf());
        cout << "New connection recieved from: " << stream->rdbuf()->remote_endpoint() << endl;
        thread th(bind(server, stream));
        th.detach();
    }
    return 0;
}

int server(shared_ptr<ip::tcp::iostream> tstream) {
    string line;
    vector<string> inLines;
    vector<string> cmdArgs;
    do {
        // Read until OVER
        line.clear();
        getline(*tstream, line);
        inLines.push_back(line);

        cmdArgs.clear();
        ring_buffer::stringToVector(line, cmdArgs, ' ', true);
        if (cmdArgs.size() > 0 && cmdArgs[0].compare("OVER") == 0) {
            // Write Responses
            for (int i = 0; i < inLines.size(); i++) {
                (*tstream) << "Echo " << inLines[i] << endl;
            }
            (*tstream) << "OVER" << endl;
            inLines.clear();
        }
    } while (!tstream->eof());
    cout << "closeing socket" << tstream->rdbuf()->remote_endpoint() << endl;
    tstream->close();
    return 0;
}