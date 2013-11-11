#include<iostream>
#include<boost/asio.hpp>
#include<boost/asio/ip/address.hpp>
#include<boost/shared_ptr.hpp>
#include"lookup.h"


using namespace std;
using namespace boost;
using namespace boost::asio;

int main(int argc, char **argv) {
    if (argc < 3) {
        usage(argv[0]);
        return 0;
    }
    string host(argv[1]);
    string service(argv[2]);
    cout << "looking up host " << host << endl;
    boost::system::error_code ec;
    io_service ios;
    boost::shared_ptr<ip::tcp::resolver::iterator> it = resolveTCP(host, service, ios, ec);
    if (ec) {
        cout << "Error resolving host:" << host << endl;
        return 0;
    }
    ip::tcp::resolver::iterator end;
    int i = 0;
    while (*it != end) {
        ip::tcp::endpoint ep((*it)->endpoint());
        boost::asio::ip::address addr = ep.address();
        cout << "endpoint[" << i << "]: " << address_to_string(addr) << " " << ep << endl;
        (*it)++;
        i++;
    }
    return 0;
}

string address_to_string(ip::address& addr) {
    ostringstream os;
    os << "{"
            << " is_loopback=" << boolalpha << addr.is_loopback() << ", "
            << "is_multicase: " << boolalpha << addr.is_multicast() << ", "
            << "is_unspecified: " << boolalpha << addr.is_unspecified() << ", "
            << "is_v4: " << boolalpha << addr.is_v4() << ", "
            << "is_v6: " << boolalpha << addr.is_v6() << ", "
            << "addr: " << addr.to_string() << "}";
    return os.str();
}

boost::shared_ptr<ip::tcp::resolver::iterator> resolveTCP(string& host, string& service, io_service& ios, boost::system::error_code& ec) {
    ip::tcp::resolver r(ios);
    ip::tcp::resolver::query q(host, service, ip::resolver_query_base::all_matching);
    boost::shared_ptr<ip::tcp::resolver::iterator> it(new ip::tcp::resolver::iterator(r.resolve(q, ec)));
    if (ec) {
        boost::shared_ptr<ip::tcp::resolver::iterator> null;
        return null;
    }
    //ip::tcp::resolver::iterator it = r.resolve(q, ec);
    cout << "it @" << it.get() << endl;
    return it;
}

int usage(char *prog) {
    bool t = true;
    bool f = false;
    cout << "usage is " << prog << " <hostname> <port>" << endl;
    cout << endl;
    return 0;
}
