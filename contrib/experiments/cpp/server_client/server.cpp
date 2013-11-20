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
        shared_ptr<ip::tcp::socket> socket(new ip::tcp::socket(ios));
        ac.accept(*socket);
        cout << "Thread: " << this_thread::get_id() << " New connection recieved from: " << socket->remote_endpoint().address() << endl;
        thread th(bind(server, socket));
        th.detach();
    }
    return 0;
}

int server(shared_ptr<ip::tcp::socket> socket) {
    ring_buffer inring(INIT_RING_SIZE);
    ring_buffer outring(INIT_RING_SIZE);
    deque<vector<string> > inlines;

    cout << "Thread: " << this_thread::get_id() << " starting server in child thread" << endl;
    char buff[SOCKBUFFSIZE + 1];
    size_t nBytes;
    system::error_code ec;
    do {
        cout << "inlines.size() = " << inlines.size() << endl;
        nBytes = socket->read_some(buffer(buff, SOCKBUFFSIZE), ec);
        cout << "inring.free_size() = " << inring.free_size() << " nBytes read from socket " << nBytes << endl;
        while (inring.free_size() < nBytes) {
            cout << "Doubleing inring size from " << inring.getDataSize() << " to ";
            inring.double_capacity();
            cout << inring.getDataSize() << endl;
        }
        if (ec != 0) break;
        cout << "Thread: " << this_thread::get_id() << " Read msg \"" << string(buff, nBytes) << "\"" << endl;
        cout << "read " << nBytes << " bytes" << endl;
        inring.write(buff, nBytes);
        //write(*socket, buffer(buff, nBytes));
        int nInputLines = inring.linesAvailable();
        cout << "found nInputLines " << nInputLines << endl;
        if (nInputLines > 0) {
            cout << "Reading" << endl;
            for (int i = 0; i < nInputLines; i++) {
                string line(inring.readLine());
                vector< string> strVecs;
                int nEntries = ring_buffer::stringToVector(line, strVecs, ' ', true);
                inlines.push_back(strVecs);
                cout << "strVec = " << ring_buffer::vectorToString(strVecs, ' ') << endl;
                if (strVecs.size() >= 1 && strVecs[0].compare("OVER") == 0) {
                    cout << "READ OVER starting write loop" << endl;
                    for (int j = 0; j < inlines.size(); j++) {
                        string lineout("ECHO: " + ring_buffer::vectorToString(inlines[j], '=') + "\n");
                        cout << "Checking if strsize " << lineout.size() << " will fit in outring " << outring.free_size() << endl;
                        while (outring.free_size() < lineout.size()) {
                            cout << "doubling size of outring from " << outring.getDataSize() << " to ";
                            outring.double_capacity();
                            cout << outring.getDataSize() << endl;
                        }

                        cout << "lineout=" << lineout << endl;
                        outring.write(lineout);
                    }
                    inlines.clear();
                    cout << "Writing to socket" << endl;
                    while (outring.used_size() > 0) {
                        cout << "found " << outring.linesAvailable() << " lines" << endl;
                        cout << "outringbuff:" << endl;
                        cout << "BeforeSockWrite: " << endl;
                        cout << outring.debug_str(true) << endl;
                        int nWrite = outring.read(buff, SOCKBUFFSIZE);
                        int sWrite = write(*socket, buffer(buff, nWrite));
                        cout << "Wrote " << nWrite << " " << sWrite << " bytes to socket: " << string(buff, nWrite) << endl
                                << "after ringout:" << endl
                                << outring.debug_str(true) << endl;
                    }
                    inlines.clear();
                    break;
                }
            }
        }
    } while (true);
    cout << "Thread: " << this_thread::get_id() << " Got error code: " << ec.message() << endl;
    cout << "Thread: " << this_thread::get_id() << " Closing socket" << endl;
    socket->close();
    cout << "Thread: " << this_thread::get_id() << " Socket closed" << endl;
    return 0;
}