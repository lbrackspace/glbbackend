#include<iostream>
#include<cstdlib>
#include<unistd.h>
#include<deque>
#include<boost/asio.hpp>
#include<boost/thread.hpp>
#include<boost/bind.hpp>
#include<boost/chrono.hpp>
#include"server.hh"
#include"GlbContainer.hh"
#include"IPRecord.hh"

using namespace std;
using namespace boost;
using namespace boost::asio;

const int SOCKBUFFSIZE = 4096;
const int STRBUFFSIZE = 4096;

const boost::chrono::microseconds sdelay(50);

int main(int argc, char **argv) {
    if (argc < 3) {
        usage(argv[0]);
        return -1;
    }
    glbMap["cname_1.rackexp.org"] = shared_ptr<GlbContainer > (new GlbContainer("cname_1.rackexp.org", GlbType::NONE));
    glbMap["cname_2.rackexp.org"] = shared_ptr<GlbContainer > (new GlbContainer("cname_2.rackexp.org", GlbType::NONE));
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

int stringToVector(const std::string& strIn, std::vector<std::string>& strVector, char delim, bool skipLF) {
    int nStrings = 0;
    char buff[STRBUFFSIZE + 1];
    buff[0] = '\0';
    int ci = 0;
    int cb = 0;
    int cl;
    int li = strIn.size();
    for (ci = 0; ci <= li; ci++) {
        cl = ci - cb;
        char ch = strIn[ci];
        if (ch == delim || ch == '\0' || cl >= STRBUFFSIZE || (skipLF && (ch == '\n' || ch == '\r'))) {
            if (cl <= 0) {
                cb = ci + 1;
                buff[0] = '\0';
                continue;
            }
            buff[ci - cb] = '\0';
            cb = ci + 1;
            strVector.push_back(std::string(buff));
            nStrings++;
            buff[0] = '\0';
            continue;
        }
        buff[ci - cb] = strIn[ci];
    }
    return nStrings;
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

void debug_domains(vector<string>& outLines) {
    shared_lock<shared_mutex> lock(glbMapMutex);
    unordered_map<string, shared_ptr<GlbContainer> >::iterator mi;
    unordered_map<string, shared_ptr<GlbContainer> >::iterator beg = glbMap.begin();
    unordered_map<string, shared_ptr<GlbContainer> >::iterator end = glbMap.end();
    for (mi = beg; mi != end; mi++) {
        ostringstream os;
        string key(mi->first);
        shared_ptr<GlbContainer> glb(mi->second);
        string val = (*glb).to_string(true);
        os << key << ":" << val;
        outLines.push_back(os.str());
    }
}

int server(shared_ptr<ip::tcp::iostream> tstream) {
    string line;
    vector<string> inLines;
    vector<string> outLines;
    vector<string> mainCmd;
    vector<string> inCmdArgs;
    do {
        // Read until OVER
        line.clear();
        getline(*tstream, line);
        inLines.push_back(line);

        mainCmd.clear();
        stringToVector(line, mainCmd, ' ', true);

        // OVER found begin processing messages
        if (mainCmd.size() > 0 && mainCmd[0].compare("OVER") == 0) {
            // Write Responses
            outLines.clear();
            for (int i = 0; i < inLines.size(); i++) { // For each message from the DMC
                inCmdArgs.clear();
                stringToVector(inLines[i], inCmdArgs, ' ', true);
                if (inCmdArgs.size() > 0 && inCmdArgs[0].compare("DEBUG_DOMAINS") == 0) {
                    debug_domains(outLines);
                }
            }

            // Write outputBuffer
            for (int i = 0; i < outLines.size(); i++) {
                (*tstream) << outLines[i] << endl;
            }
            (*tstream) << "OVER" << endl;
            inLines.clear();
        }
    } while (!tstream->eof());
    cout << "closeing socket" << tstream->rdbuf()->remote_endpoint() << endl;
    tstream->close();
    return 0;
}
