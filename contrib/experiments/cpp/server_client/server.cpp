#include<iostream>
#include<cstdlib>
#include<unistd.h>

#include<boost/asio.hpp>
#include<boost/thread.hpp>
#include<boost/bind.hpp>

#include"server.h"
#include"GlbContainer.h"

using namespace std;
using namespace boost;
using namespace boost::asio;

const int SOCKBUFFSIZE = 255;

int main(int argc, char **argv) {
    if (argc < 3) {
        usage(argv[0]);
        return -1;
    }
    glbMapLock.lock();
    glbMapLock.unlock();
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
        shared_ptr<ip::tcp::socket> socket(new ip::tcp::socket(ios));
        ac.accept(*socket);
        cout << "Thread: " << this_thread::get_id() << " New connection recieved from: " << socket->remote_endpoint().address() << endl;
        thread th(bind(server, socket));
        th.detach();
    }
    return 0;
}

int server(shared_ptr<ip::tcp::socket> socket) {
    cout << "Thread: " << this_thread::get_id() << " starting server in child thread" << endl;
    char buff[SOCKBUFFSIZE + 1];
    size_t nBytes;
    system::error_code ec;
    do {
        nBytes = socket->read_some(buffer(buff, SOCKBUFFSIZE), ec);
        if (ec != 0) break;
        cout << "Thread: " << this_thread::get_id() << " Read msg \"" << string(buff, nBytes) << "\"" << endl;
        write(*socket, buffer(buff, nBytes));
    } while (true);
    cout << "Thread: " << this_thread::get_id() << " Got error code: " << ec.message() << endl;
    cout << "Thread: " << this_thread::get_id() << " Closing socket" << endl;
    socket->close();
    cout << "Thread: " << this_thread::get_id() << " Socket closed" << endl;
}