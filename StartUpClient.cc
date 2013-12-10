#include "StartUpClient.hh"

#include <boost/asio.hpp>
#include <sstream>
#include "ServerJsonBuilder.hh"

bool StartUpClient::connect() {
    using namespace std;
    using namespace boost;
    using namespace boost::asio;

    io_service ios;
    ostringstream os;
    ServerJsonBuilder jb;
    jb.setType("pdns");
    os << port;
    string portStr(os.str());
    shared_ptr<ip::tcp::iostream> socket_stream(new ip::tcp::iostream(host, portStr));
    if (!(*(socket_stream))) {
        cout << "Error connecting to host:" << host << " port:" << port << std::endl;
        return false;
    }
    (*socket_stream) << jb.toJson() << "EOT";
    (*socket_stream).flush();
    socket_stream->close();
    return true;
}
