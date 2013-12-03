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
#include<cstdlib>
#include<stdexcept>
#include<sys/types.h>
#include<unistd.h>
#include"server.hh"
#include"GlbContainer.hh"
#include"IPRecord.hh"
#include"GLBCollection.hh"
#include"SOAContainer.hh"

using namespace std;
using namespace boost;
using namespace boost::asio;

const int SOCKBUFFSIZE = 4096;
const int STRBUFFSIZE = 4096;

const boost::chrono::microseconds sdelay(50);

bool cmdMatch(int nArgs, const vector<string>& sv, string expected) {
    if (sv.size() >= nArgs && sv[0].compare(expected) == 0) {
        return true;
    }
    return false;
}

int splitStr(vector<string>& svOut, string strIn, string delim) {
    boost::algorithm::replace_all(strIn, "\n", "");
    boost::algorithm::replace_all(strIn, "\r", "");
    boost::algorithm::split(svOut, strIn, boost::algorithm::is_any_of(delim), boost::token_compress_on);
    return svOut.size();
}

string joinStr(const vector<string>& strIn, string delim) {
    return boost::algorithm::join(strIn, delim);
}

void start_server_thread(std::string ip_addr_str, int port) {
    boost::thread th(bind(listener, ip_addr_str, port));
    th.detach();
}

int listener(string ip_addr_str, int port) {
    io_service ios;
    cout << "Resolving ip Address " << ip_addr_str << " for port " << port << endl;
    ip::tcp::endpoint ep(ip::address::from_string(ip_addr_str), port);
    cout << "Resolved endpoint to address: " << ep.address() << endl;
    cout << "Listening on addr: " << ep.address() << " port " << port << endl;
    cout << "pdns pid: " << getpid() << endl;
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

bool decodeIP(const string inputStr, string &errorMsg, int &ipt, int &ttl, string &addr, string &attr) {
    vector<string> ipVec;
    int nItems = splitStr(ipVec, inputStr, "-");
    if (nItems < 4) {
        errorMsg = string(":epected4Vals");
        return false;
    }
    ipt = std::atoi(ipVec[0].c_str());
    ttl = std::atoi(ipVec[1].c_str());
    addr = ipVec[2];
    attr = ipVec[3];
    if (ttl <= 0) {
        errorMsg = ":ttl_less_than_0";
        return false;
    }
    if (ipt == 4) {
        ipt = IPRecordType::IPv4;
    } else if (ipt == 6) {
        ipt = IPRecordType::IPv6;
    } else {
        errorMsg = ":Unknown_ipType";
        return false;
    }
    return true;
}

void snapshot_domain(std::vector<std::string> &outLines, std::string line) {
    vector<string> inArgs;
    vector<string>ipVec;
    vector<IPRecord> ips;
    int ipTTL;
    int ipType;
    string ipAddr;
    string ipAttr;
    string errorMsg;
    int nArgs = splitStr(inArgs, line, " ");
    if (nArgs < 2) {
        outLines.push_back("SNAPSHOT FAILED: cname required");
        return;
    }
    ostringstream os;
    string cname = inArgs[1];
    int li = inArgs.size();
    for (int i = 2; i < li; i++) {
        string curr_ip(inArgs[i]);
        if (!decodeIP(curr_ip, errorMsg, ipType, ipTTL, ipAddr, ipAttr)) {
            os << " r" << curr_ip << errorMsg;
            continue;
        }
        ips.push_back(IPRecord(ipType, ipAddr, ipTTL));
        os << " a" << curr_ip;
    }
    // Find the glb from the map
    shared_ptr<GlbContainer> glb;
    {
        shared_lock<shared_mutex> lock(glbMapMutex);
        unordered_map<string, shared_ptr<GlbContainer> >::iterator it = glbMap.find(cname);
        if (glbMap.find(cname) == glbMap.end()) {
            outLines.push_back("SNAPSHOT FAILED: " + cname + " Not found");
            return;
        }
        glb = shared_ptr<GlbContainer > (it->second);
    }
    (*glb).setIPs(ips);
    outLines.push_back("SNAPSHOT PASSED: " + cname + " " + os.str());
}

void del_domain(vector<string>& outLines, string line) {
    vector<string> inArgs;
    int nArgs = splitStr(inArgs, line, " ");
    string cname = inArgs[1];
    if (nArgs < 2) {
        outLines.push_back("DEL_DOMAIN FAILED: Needed cname argument for command");
        return;
    }
    {
        lock_guard<shared_mutex> lock(glbMapMutex);
        unordered_map<string, shared_ptr<GlbContainer> >::iterator it = glbMap.find(cname);
        if (it == glbMap.end()) {
            outLines.push_back("DEL_DOMAIN FAILED: " + cname + " not found");
            return;
        }
        glbMap.erase(cname);
        outLines.push_back("DEL_DOMAIN PASSED: " + cname);
    }
}

void add_domain(vector<string>& outLines, string line) {
    vector<string> inArgs;
    int nArgs = splitStr(inArgs, line, " ");
    if (nArgs < 3) {
        outLines.push_back("ADD_DOMAIN FAILED: Needed cname and algo argument for command arguments for command");
        return;
    }
    string cname = inArgs[1];
    string algoName = inArgs[2];
    int glbType = strToGlbType(algoName);
    if (glbType < 0) {
        outLines.push_back("ADD_DOMAIN FAILED: " + cname + " Unknown Algo " + algoName);
        return;
    }
    shared_ptr<GlbContainer> glb(new GlbContainer(cname, glbType));
    {
        lock_guard<shared_mutex> lock(glbMapMutex);
        if (glbMap.find(cname) != glbMap.end()) {
            outLines.push_back("ADD_DOMAIN FAILED: " + cname + " already exists can not add");
            return;
        }
        glbMap[cname] = glb;
    }
    outLines.push_back("ADD_DOMAIN PASSED: " + cname);
}

void clr_counts(std::vector<std::string>& outLines, std::string line) {
    vector<string>args;
    int nArgs = splitStr(args, line, " ");
    for (int i = 1; i < nArgs; i++) {
        string cname = args[i];
        {
            shared_lock<shared_mutex> lock(glbMapMutex);
            unordered_map<string, shared_ptr<GlbContainer> >::iterator it = glbMap.find(cname);
            if (it == glbMap.end()) {
                outLines.push_back("CLR_COUNTS " + cname + " FAILED: cname doesn't exist");
                continue;
            }
            (it->second)->clrNLookups();
            outLines.push_back("CLR_COUNTS " + cname + " PASSED");
        }
    }
}

void get_counts(std::vector<std::string>& outLines, std::string line) {
    vector<string> args;
    int nArgs = splitStr(args, line, " ");
    if (nArgs > 1) {
        int as = args.size();
        for (int i = 1; i < as; i++) {
            shared_lock<shared_mutex> lock(glbMapMutex);
            string cname(args[i]);
            unordered_map<string, shared_ptr<GlbContainer> >::iterator it = glbMap.find(cname);
            if (it == glbMap.end()) {
                outLines.push_back("COUNTS " + cname + " FAILED: cname doesn't exist");
                continue;
            }
            ostringstream os;
            os << "COUNTS " + cname << " PASSED: " << (it->second)->getNLookups();
            outLines.push_back(os.str());
        }
    } else {
        shared_lock<shared_mutex> lock(glbMapMutex);
        unordered_map<string, shared_ptr<GlbContainer> >::iterator it;
        for (it = glbMap.begin(); it != glbMap.end(); it++) {
            ostringstream os;
            os << "COUNT " << it->first << " PASSED: " << (it->second)->getNLookups();
            outLines.push_back(os.str());
        }
    }
}

void debug_domains(vector<string>& outLines, string line) {
    int nLoops = 1;
    vector<string> args;
    if (splitStr(args, line, " ") >= 2) {
        nLoops = std::atoi(args[1].c_str());
    }
    bool showFull = false;
    if (args.size() >= 3 && args[2].compare("FULL") == 0) {
        showFull = true;
    }
    shared_lock<shared_mutex> lock(glbMapMutex);
    unordered_map<string, shared_ptr<GlbContainer> >::iterator mi;
    unordered_map<string, shared_ptr<GlbContainer> >::iterator beg = glbMap.begin();
    unordered_map<string, shared_ptr<GlbContainer> >::iterator end = glbMap.end();
    for (int i = 0; i < nLoops; i++) {
        for (mi = beg; mi != end; mi++) {
            string key(mi->first);
            shared_ptr<GlbContainer> glb(mi->second);
            string val = (*glb).to_string(showFull);
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
        try {
            line.clear();
            getline(*tstream, line);
            if (!(*tstream)) {
                cout << " ERROR: " << (*tstream).error().message() << endl;
                break;
            }
            mainCmd.clear();
            splitStr(mainCmd, line, " ");

            // If OVER found begin processing messages
            if (mainCmd.size() > 0 && mainCmd[0].compare("OVER") == 0) {
                clock_t startTime = clock();
                // Write Responses
                outLines.clear();
                for (int i = 0; i < inLines.size(); i++) { // For each message from the DMC
                    inCmdArgs.clear();
                    splitStr(inCmdArgs, inLines[i], " ");
                    if (cmdMatch(1, inCmdArgs, "DEBUG_DOMAINS")) {
                        debug_domains(outLines, inLines[i]);
                    } else if (cmdMatch(3, inCmdArgs, "ADD_DOMAIN")) {
                        add_domain(outLines, inLines[i]);
                    } else if (cmdMatch(2, inCmdArgs, "DEL_DOMAIN")) {
                        del_domain(outLines, inLines[i]);
                    } else if (cmdMatch(2, inCmdArgs, "SNAPSHOT")) {
                        snapshot_domain(outLines, inLines[i]);
                    } else if (cmdMatch(1, inCmdArgs, "COUNTS")) {
                        get_counts(outLines, inLines[i]);
                    } else if (cmdMatch(1, inCmdArgs, "CLR_COUNTS")) {
                        clr_counts(outLines, inLines[i]);
                    } else {
                        unknown_command(outLines, inLines[i]);
                    }
                }
                clock_t endTime = clock();
                double processTime = static_cast<double> (endTime - startTime) / CLOCKS_PER_SEC;
                // Write outputBuffer
                startTime = clock();
                outLines.push_back("OVER");
                (*tstream) << joinStr(outLines, "\n") << endl;
                if (!(*tstream)) {
                    cout << " ERROR: " << (*tstream).error().message() << endl;
                    break;
                }

                inLines.clear();
                outLines.clear();
                //clock_t endTime = clock();
                endTime = clock();
                double writeTime = static_cast<double> (endTime - startTime) / CLOCKS_PER_SEC;
                cout << "PROCESS LOOP FINISHED IN " << processTime << " seconds. Tool " << writeTime << " seconds to write output." << endl;
            } else {
                inLines.push_back(line); // Otherwise store the line on the input buffer
            }
        } catch (std::exception& ex) {
            cout << "Exception: " << ex.what() << endl;
            break;
        }
    } while (!tstream->eof());
    try {
        cout << "closeing socket" << tstream->rdbuf()->remote_endpoint() << endl;
    } catch (std::exception& ex) {
        cout << "warning remote child socket closed prematurly: " << ex.what() << endl;
    }
    try {
        tstream->close();
    } catch (std::exception& ex) {
        cout << "exception: " << ex.what() << " while trying to close child socket" << endl;
    }
    return 0;
}

