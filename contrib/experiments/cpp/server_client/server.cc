#include<iostream>
#include<cstdlib>
#include<unistd.h>
#include<ctime>
#include<deque>
#include<boost/asio.hpp>
#include<boost/thread.hpp>
#include<boost/bind.hpp>
#include<boost/chrono.hpp>
#include<boost/algorithm/string.hpp>
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

bool cmdMatch(int nArgs, const vector<string>& sv, string expected) {
    if (sv.size() >= nArgs && sv[0].compare(expected) == 0) {
        return true;
    }
    return false;
}

int splitStr(vector<string>& svOut, string strIn) {
    boost::algorithm::replace_all(strIn, "\n", "");
    boost::algorithm::replace_all(strIn, "\r", "");
    boost::algorithm::split(svOut, strIn, boost::algorithm::is_any_of(" "), boost::token_compress_on);
    return svOut.size();
}

string joinStr(const vector<string>& strIn, string delim) {
    return boost::algorithm::join(strIn, delim);
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

void debug_domains(vector<string>& outLines, string line) {
    int nLoops = 1;
    vector<string> args;
    if (splitStr(args, line) >= 2) {
        nLoops = std::atoi(args[1].c_str());
    }
    shared_lock<shared_mutex> lock(glbMapMutex);
    unordered_map<string, shared_ptr<GlbContainer> >::iterator mi;
    unordered_map<string, shared_ptr<GlbContainer> >::iterator beg = glbMap.begin();
    unordered_map<string, shared_ptr<GlbContainer> >::iterator end = glbMap.end();
    for (int i = 0; i < nLoops; i++) {
        for (mi = beg; mi != end; mi++) {
            string key(mi->first);
            shared_ptr<GlbContainer> glb(mi->second);
            string val = (*glb).to_string(false);
            outLines.push_back(key + ":" + val);
        }
    }
}

void unknown_command(vector<string>&outLines, string line) {
    ostringstream os;
    os << "UNKNOWN COMMAND: " << line;
    outLines.push_back(os.str());
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

        mainCmd.clear();
        splitStr(mainCmd, line);

        // If OVER found begin processing messages
        if (mainCmd.size() > 0 && mainCmd[0].compare("OVER") == 0) {
            clock_t startTime = clock();
            // Write Responses
            outLines.clear();
            for (int i = 0; i < inLines.size(); i++) { // For each message from the DMC
                inCmdArgs.clear();
                splitStr(inCmdArgs, inLines[i]);
                if (cmdMatch(1, inCmdArgs, "DEBUG_DOMAINS")) {
                    debug_domains(outLines, inLines[i]);
                } else {
                    unknown_command(outLines, inLines[i]);
                }
            }
            clock_t endTime = clock();
            double processTime = static_cast<double> (endTime - startTime) / CLOCKS_PER_SEC;
            // Write outputBuffer
            startTime = clock();
            //for (int i = 0; i < outLines.size(); i++) {
            //    (*tstream) << outLines[i] << endl;
            //}
            outLines.push_back("OVER");
            (*tstream) << joinStr(outLines, "\n") << endl;
            //            (*tstream) << "OVER" << endl;
            inLines.clear();
            //clock_t endTime = clock();
            endTime = clock();
            double writeTime = static_cast<double> (endTime - startTime) / CLOCKS_PER_SEC;
            cout << "PROCESS LOOP FINISHED IN " << processTime << " seconds. Tool " << writeTime << " seconds to write output." << endl;
        } else {
            inLines.push_back(line); // Otherwise store the line on the input buffer
        }
    } while (!tstream->eof());
    cout << "closeing socket" << tstream->rdbuf()->remote_endpoint() << endl;
    tstream->close();
    return 0;
}
