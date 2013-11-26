#include<cstdlib>
#include<iostream>
#include"server.hh"

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
    cout << "pid = " << getpid() << endl;
    string ip_addr_str(argv[1]);
    int port = std::atoi(argv[2]);
    listener(ip_addr_str, port);
    return 0;
}

