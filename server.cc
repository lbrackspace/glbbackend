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
#include <pdns/ext/rapidjson/include/rapidjson/stringbuffer.h>
#include <pdns/ext/rapidjson/include/rapidjson/stringbuffer.h>
#include <pdns/ext/rapidjson/include/rapidjson/filestream.h>
#include <pdns/ext/rapidjson/include/rapidjson/writer.h>
#include<cstdlib>
#include<stdexcept>
#include<sys/types.h>
#include<unistd.h>
#include"server.hh"
#include"GlbContainer.hh"
#include"IPRecord.hh"
#include"GLBCollection.hh"
#include"SOAContainer.hh"
#include "contrib/experiments/cpp/maindemo/rapidjson/document.h"
#include "contrib/experiments/cpp/maindemo/rapidjson/stringbuffer.h"

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

string joinStr(const vector<string> &strIn, string delim, int start_i) {
    vector<string> strVec;
    int ss = strIn.size();
    for (int i = start_i; i < ss; i++) {
        strVec.push_back(strIn[i]);
    }
    return joinStr(strVec, " ");
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
        errorMsg = string("-epected4Vals");
        return false;
    }
    ipt = std::atoi(ipVec[0].c_str());
    ttl = std::atoi(ipVec[1].c_str());
    addr = ipVec[2];
    attr = ipVec[3];
    if (ttl <= 0) {
        errorMsg = "-ttl_less_than_0";
        return false;
    }
    if (ipt == 4) {
        ipt = IPRecordType::IPv4;
    } else if (ipt == 6) {
        ipt = IPRecordType::IPv6;
    } else {
        errorMsg = "-Unknown_ipType";
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
        outLines.push_back("SNAPSHOT FAILED: fqdn required");
        return;
    }
    ostringstream os;
    string fqdn = inArgs[1];
    bool snapPassed = true;
    int as = inArgs.size();
    for (int i = 2; i < as; i++) {
        string curr_ip(inArgs[i]);
        if (!decodeIP(curr_ip, errorMsg, ipType, ipTTL, ipAddr, ipAttr)) {
            os << " r-" << curr_ip << "-" << errorMsg;
            snapPassed = false;
            continue;
        }
        ips.push_back(IPRecord(ipType, ipAddr, ipTTL));
        os << " a-" << curr_ip;
    }
    // Find the glb from the map
    shared_ptr<GlbContainer> glb;
    {
        shared_lock<shared_mutex> lock(glbMapMutex);
        unordered_map<string, shared_ptr<GlbContainer> >::iterator it = glbMap.find(fqdn);
        string snapPassedMsg = (snapPassed) ? "PASSED:" : "FAILED:";
        if (glbMap.find(fqdn) == glbMap.end()) {
            outLines.push_back("SNAPSHOT " + snapPassedMsg + fqdn + " Not found");
            return;
        }
        glb = shared_ptr<GlbContainer > (it->second);
    }
    (*glb).setIPs(ips);
    outLines.push_back("SNAPSHOT PASSED: " + fqdn + " " + os.str());
}

void del_domain(vector<string>& outLines, string line) {
    vector<string> inArgs;
    int nArgs = splitStr(inArgs, line, " ");
    string fqdn = inArgs[1];
    if (nArgs < 2) {
        outLines.push_back("DEL_DOMAIN FAILED: Needed fqdn argument for command");
        return;
    }
    {
        lock_guard<shared_mutex> lock(glbMapMutex);
        unordered_map<string, shared_ptr<GlbContainer> >::iterator it = glbMap.find(fqdn);
        if (it == glbMap.end()) {
            outLines.push_back("DEL_DOMAIN FAILED: " + fqdn + " not found");
            return;
        }
        glbMap.erase(fqdn);
        outLines.push_back("DEL_DOMAIN PASSED: " + fqdn);
    }
}

void add_domain(vector<string>& outLines, string line) {
    using namespace rapidjson;
    ServerJsonBuilder jb;
    vector<string> inArgs;
    string errorMsg;
    jb.setType("ADD_DOMAIN");
    int nArgs = splitStr(inArgs, line, " ");
    if (nArgs < 3) {
        jb.setStatus("FAILED");
        jb.setError("Needed fqdn and algo argument for command arguments for command");
        outLines.push_back(jb.to_json());
        return;
    }
    string fqdn = inArgs[1];
    string algoName = inArgs[2];
    jb.setFqdn(fqdn);
    int glbType = strToGlbType(algoName);
    if (glbType < 0) {
        jb.setStatus("FAILED");
        jb.setError("Unknown Algo: " + algoName);
        outLines.push_back(jb.to_json());
        return;
    }
    shared_ptr<GlbContainer> glb(new GlbContainer(fqdn, glbType));
    {
        lock_guard<shared_mutex> lock(glbMapMutex);
        if (glbMap.find(fqdn) != glbMap.end()) {
            jb.setStatus("FAILED");
            jb.setError("fqdn already exists");
            outLines.push_back(jb.to_json());
            return;
        }
        glbMap[fqdn] = glb;
    }
    jb.setStatus("PASSED");
    outLines.push_back(jb.to_json());
}

void set_soa(std::vector<std::string> &outLines, std::string line) {
    using namespace rapidjson;
    vector<string> args;
    int nArgs = splitStr(args, line, " ");
    if (nArgs < 3) {
        ostringstream os;
        os << "SET_SOA FAILED: expected atleast 3 arguments but found only " << nArgs;
        outLines.push_back(os.str());
        return;
    }
    string baseFQDNValue = args[1];
    string soaValue = joinStr(args, " ", 2);
    setGlobalSOARecord(soaValue, baseFQDNValue);
    outLines.push_back("SET_SOA PASSED:");
    return;
}

void set_ns(std::vector<std::string> &outLines, std::string line) {
    vector<string> args;
    int nArgs = splitStr(args, line, " ");
    ServerJsonBuilder jb;
    jb.setType("SET_NS");
    if (nArgs < 2) {
        jb.setStatus("FAILED");
        ostringstream os;
        os << nArgs;
        os << "expected atleast 2 parameters only got " << nArgs;
        jb.setError(os.str());
        outLines.push_back(jb.to_json());
        return;
    }
    vector<string> newRecords;
    for (int i = 1; i < nArgs; i++) {
        newRecords.push_back(args[i]);
        jb.addNSRecord(args[i]);
    }
    setNSRecords(newRecords);
    jb.setStatus("PASSED");
    outLines.push_back(jb.to_json());
    return;
}

void clr_counts(std::vector<std::string>& outLines, std::string line) {
    vector<string>args;
    int nArgs = splitStr(args, line, " ");
    bool failed = false;
    ServerJsonBuilder jb;
    jb.setType("CLR_COUNTS");
    if (nArgs > 1) {
        shared_lock<shared_mutex> lock(glbMapMutex);
        for (int i = 1; i < nArgs; i++) {
            string fqdn = args[i];
            unordered_map<string, shared_ptr<GlbContainer> >::iterator it = glbMap.find(fqdn);
            if (it == glbMap.end()) {
                jb.addError("fqdn" + fqdn + " doesn't exist");
                failed = true;
                continue;
            }
            (it->second)->clrNLookups();
        }
        if (failed) {
            jb.setStatus("FAILED");
        } else {
            jb.setStatus("PASSED");
        }
        outLines.push_back(jb.to_json());
    } else {
        shared_lock<shared_mutex> lock(glbMapMutex);
        unordered_map<string, shared_ptr<GlbContainer> >::iterator it;
        for (it = glbMap.begin(); it != glbMap.end(); it++) {
            it->second->clrNLookups();
        }
        jb.setStatus("PASSED");
        outLines.push_back(jb.to_json());
    }
}

void get_counts(std::vector<std::string>& outLines, std::string line) {
    vector<string> args;
    ServerJsonBuilder jb;
    jb.setType("GET_COUNTS");
    int nArgs = splitStr(args, line, " ");
    bool failed = false;
    if (nArgs > 1) {
        for (int i = 1; i < nArgs; i++) {
            shared_lock<shared_mutex> lock(glbMapMutex);
            string fqdn(args[i]);
            unordered_map<string, shared_ptr<GlbContainer> >::iterator it = glbMap.find(fqdn);
            if (it == glbMap.end()) {
                jb.setStatus("FAILED");
                jb.addError(string("fqdn " + fqdn + " doesn't exist"));
                failed = true;
                continue;
            }
            ostringstream os;
            jb.addCount(fqdn, it->second->getNLookups());
        }
        if (failed) {
            jb.setStatus("FAILED");
        } else {
            jb.setStatus("PASSED");
        }
        outLines.push_back(jb.to_json());
    } else {
        shared_lock<shared_mutex> lock(glbMapMutex);
        unordered_map<string, shared_ptr<GlbContainer> >::iterator it;
        for (it = glbMap.begin(); it != glbMap.end(); it++) {
            jb.addCount(it->first, it->second->getNLookups());
        }
        jb.setStatus("PASSED");
        outLines.push_back(jb.to_json());
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
    ServerJsonBuilder jb;
    jb.setType("UNKNOWN COMMAND");
    jb.setError(line);
    outLines.push_back(jb.to_json());
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
                    } else if (cmdMatch(3, inCmdArgs, "SET_SOA")) {
                        set_soa(outLines, inLines[i]);
                    } else if (cmdMatch(2, inCmdArgs, "SET_NS")) {
                        set_ns(outLines, inLines[i]);
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

