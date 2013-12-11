#include <cstdlib>
#include <ctime>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <deque>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/chrono.hpp>
#include <boost/algorithm/string.hpp>
#include <stdexcept>

#include "GlbContainer.hh"
#include "IPRecord.hh"
#include "GLBCollection.hh"
#include "SOAContainer.hh"
#include "ServerJsonBuilder.hh"
#include "GLBCommandServer.hh"

using namespace std;
using namespace boost;
using namespace boost::asio;

const int SOCKBUFFSIZE = 4096;
const int STRBUFFSIZE = 4096;

const boost::chrono::microseconds sdelay(50);

GLBCommandServer::GLBCommandServer(const std::string ip, int port) {
  this->m_ip_address = ip;
  this->m_port = port;
}

const std::string& GLBCommandServer::getIpAddress() const {
  return this->m_ip_address;
}

void GLBCommandServer::setIpAddress(const std::string& ip) {
  this->m_ip_address = m_ip_address;
}

int GLBCommandServer::getPort() const {
  return m_port;
}

void GLBCommandServer::setPort(int port) {
  this->m_port = port;
}

void GLBCommandServer::addDomain(std::vector<std::string>& outLines,
                                 std::string line) {

  ServerJsonBuilder jb;
  vector<string> inArgs;
  string errorMsg;
  jb.setType("ADD_DOMAIN");
  int nArgs = splitStr(inArgs, line, " ");
  if (nArgs < 3) {
    jb.setStatus("FAILED");
    jb.setError(
        "Needed fqdn and algo argument for command arguments for command");
    outLines.push_back(jb.toJson());
    return;
  }
  string fqdn = inArgs[1];
  string algoName = inArgs[2];
  jb.setFqdn(fqdn);
  int glbType = strToGlbType(algoName);
  if (glbType < 0) {
    jb.setStatus("FAILED");
    jb.setError("Unknown Algo: " + algoName);
    outLines.push_back(jb.toJson());
    return;
  }
  shared_ptr<GlbContainer> glb(new GlbContainer(fqdn, glbType));
  {
    lock_guard<shared_mutex> lock(glbMapMutex);
    if (glbMap.find(fqdn) != glbMap.end()) {
      jb.setStatus("FAILED");
      jb.setError("fqdn already exists");
      outLines.push_back(jb.toJson());
      return;
    }
    glbMap[fqdn] = glb;
  }
  jb.setStatus("PASSED");
  outLines.push_back(jb.toJson());
}

void GLBCommandServer::delDomain(std::vector<std::string>& outLines,
                                 std::string line) {
  vector<string> inArgs;
  int nArgs = splitStr(inArgs, line, " ");
  string fqdn = inArgs[1];
  ServerJsonBuilder jb;
  jb.setType("DEL_DOMAIN");
  if (nArgs < 2) {
    jb.setStatus("FAILED");
    jb.setError("DEL_DOMAIN FAILED: Needed fqdn argument for command");
    outLines.push_back(jb.toJson());
    return;
  }
  {
    lock_guard<shared_mutex> lock(glbMapMutex);
    jb.setFqdn(fqdn);
    unordered_map<string, shared_ptr<GlbContainer> >::iterator it = glbMap.find(
        fqdn);
    if (it == glbMap.end()) {
      jb.setStatus("FAILED");
      jb.setError(fqdn + " not found");
      outLines.push_back(jb.toJson());
      return;
    }
    glbMap.erase(fqdn);
    jb.setStatus("PASSED");
    outLines.push_back(jb.toJson());
  }
}

void GLBCommandServer::setSOA(std::vector<std::string>& outLines,
                              std::string line) {

  vector<string> args;
  ServerJsonBuilder jb;
  jb.setType("SET_SOA");
  int nArgs = splitStr(args, line, " ");
  if (nArgs < 3) {
    ostringstream os;
    os << "expected atleast 3 arguments but found only " << nArgs;
    jb.setStatus("FAILED");
    jb.setError(os.str());
    outLines.push_back(jb.toJson());
    return;
  }
  string baseFQDNValue = args[1];
  string soaValue = joinStr(args, " ", 2);
  setGlobalSOARecord(soaValue, baseFQDNValue);
  jb.setStatus("PASSED");
  outLines.push_back(jb.toJson());
  return;
}

void GLBCommandServer::setNS(std::vector<std::string>& outLines,
                             std::string line) {
  vector<string> args;
  int nArgs = splitStr(args, line, " ");
  ServerJsonBuilder jb;
  jb.setType("SET_NS");
  if (nArgs < 2) {
    jb.setStatus("FAILED");
    ostringstream os;
    os << "expected atleast 2 parameters only got " << nArgs;
    jb.setError(os.str());
    outLines.push_back(jb.toJson());
    return;
  }
  vector<string> newRecords;
  for (int i = 1; i < nArgs; i++) {
    newRecords.push_back(args[i]);
    jb.addNSRecord(args[i]);
  }
  setNSRecords(newRecords);
  jb.setStatus("PASSED");
  outLines.push_back(jb.toJson());
  return;
}

void GLBCommandServer::debugDomains(std::vector<std::string>& outLines,
                                    std::string line) {
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
  unordered_map<string, shared_ptr<GlbContainer> >::iterator beg =
      glbMap.begin();
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

void GLBCommandServer::unknownCommand(std::vector<std::string>& outLines,
                                      std::string line) {
  ServerJsonBuilder jb;
  jb.setType("UNKNOWN COMMAND");
  jb.setError(line);
  outLines.push_back(jb.toJson());
}

bool GLBCommandServer::decodeIP(const std::string inputStr,
                                std::string& errorMsg, int& ipt, int& ttl,
                                std::string& addr, std::string& attr) {
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

void GLBCommandServer::snapshotDomain(std::vector<std::string>& outLines,
                                      std::string line) {
  vector<string> inArgs;
  vector<IPRecord> ips;
  int ipTTL;
  int ipType;
  string ipAddr;
  string ipAttr;
  string errorMsg;
  string snapPassedMsg;
  ServerJsonBuilder jb;
  jb.setType("SNAPSHOT");
  int nArgs = splitStr(inArgs, line, " ");
  if (nArgs < 2) {
    jb.setStatus("FAILED");
    jb.addError("fqdn required");
    return;
  }
  string fqdn = inArgs[1];
  jb.setFqdn(fqdn);
  bool snapPassed = true;
  int as = inArgs.size();
  for (int i = 2; i < as; i++) {
    string curr_ip(inArgs[i]);
    if (!decodeIP(curr_ip, errorMsg, ipType, ipTTL, ipAddr, ipAttr)) {
      jb.addIp(curr_ip, errorMsg);
      snapPassed = false;
      continue;
    }
    ips.push_back(IPRecord(ipType, ipAddr, ipTTL));
    jb.addIp(curr_ip, "");
  }
  // Find the glb from the map
  shared_ptr<GlbContainer> glb;
  {
    shared_lock<shared_mutex> lock(glbMapMutex);
    unordered_map<string, shared_ptr<GlbContainer> >::iterator it = glbMap.find(
        fqdn);
    snapPassedMsg = (snapPassed) ? "PASSED" : "FAILED";
    if (glbMap.find(fqdn) == glbMap.end()) {
      jb.setStatus("FAILED");
      jb.setError(fqdn + " Not found");
      outLines.push_back(jb.toJson());
      return;
    }
    glb = shared_ptr<GlbContainer>(it->second);
  }
  (*glb).setIPs(ips);
  jb.setStatus(snapPassedMsg);
  outLines.push_back(jb.toJson());
}

void GLBCommandServer::getCounts(std::vector<std::string>& outLines,
                                 std::string line) {
  vector<string> args;
  ServerJsonBuilder jb;
  jb.setType("GET_COUNTS");
  int nArgs = splitStr(args, line, " ");
  bool failed = false;
  if (nArgs > 1) {
    for (int i = 1; i < nArgs; i++) {
      shared_lock<shared_mutex> lock(glbMapMutex);
      string fqdn(args[i]);
      unordered_map<string, shared_ptr<GlbContainer> >::iterator it = glbMap
          .find(fqdn);
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
    outLines.push_back(jb.toJson());
  } else {
    shared_lock<shared_mutex> lock(glbMapMutex);
    unordered_map<string, shared_ptr<GlbContainer> >::iterator it;
    for (it = glbMap.begin(); it != glbMap.end(); it++) {
      jb.addCount(it->first, it->second->getNLookups());
    }
    jb.setStatus("PASSED");
    outLines.push_back(jb.toJson());
  }
}

void GLBCommandServer::clearCounts(std::vector<std::string>& outLines,
                                   std::string line) {
  vector<string> args;
  int nArgs = splitStr(args, line, " ");
  bool failed = false;
  ServerJsonBuilder jb;
  jb.setType("CLR_COUNTS");
  if (nArgs > 1) {
    shared_lock<shared_mutex> lock(glbMapMutex);
    for (int i = 1; i < nArgs; i++) {
      string fqdn = args[i];
      unordered_map<string, shared_ptr<GlbContainer> >::iterator it = glbMap
          .find(fqdn);
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
    outLines.push_back(jb.toJson());
  } else {
    shared_lock<shared_mutex> lock(glbMapMutex);
    unordered_map<string, shared_ptr<GlbContainer> >::iterator it;
    for (it = glbMap.begin(); it != glbMap.end(); it++) {
      it->second->clrNLookups();
    }
    jb.setStatus("PASSED");
    outLines.push_back(jb.toJson());
  }
}

int GLBCommandServer::listener(std::string ip, int port) {
  io_service ios;
  cout << "Resolving ip Address " << ip << " for port " << port
       << endl;
  ip::tcp::endpoint ep(ip::address::from_string(ip), port);
  cout << "Resolved endpoint to address: " << ep.address() << endl;
  cout << "Listening on addr: " << ep.address() << " port " << port << endl;
  cout << "pdns pid: " << getpid() << endl;
  ip::tcp::acceptor ac(ios, ep);

  while (true) {
    shared_ptr<ip::tcp::iostream> socket_stream(new ip::tcp::iostream());
    ac.accept(*socket_stream->rdbuf());
    cout << "New connection recieved from: "
         << socket_stream->rdbuf()->remote_endpoint() << endl;
    thread th(bind(&GLBCommandServer::server, this, socket_stream));
    th.detach();
  }
  return 0;
}

int GLBCommandServer::server(
    boost::shared_ptr<boost::asio::ip::tcp::iostream> tstream) {
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
        for (int i = 0; i < inLines.size(); i++) {  // For each message from the DMC
          inCmdArgs.clear();
          splitStr(inCmdArgs, inLines[i], " ");
          if (cmdMatch(1, inCmdArgs, "DEBUG_DOMAINS")) {
            debugDomains(outLines, inLines[i]);
          } else if (cmdMatch(3, inCmdArgs, "ADD_DOMAIN")) {
            addDomain(outLines, inLines[i]);
          } else if (cmdMatch(2, inCmdArgs, "DEL_DOMAIN")) {
            delDomain(outLines, inLines[i]);
          } else if (cmdMatch(2, inCmdArgs, "SNAPSHOT")) {
            snapshotDomain(outLines, inLines[i]);
          } else if (cmdMatch(1, inCmdArgs, "COUNTS")) {
            getCounts(outLines, inLines[i]);
          } else if (cmdMatch(1, inCmdArgs, "CLR_COUNTS")) {
            clearCounts(outLines, inLines[i]);
          } else if (cmdMatch(3, inCmdArgs, "SET_SOA")) {
            setSOA(outLines, inLines[i]);
          } else if (cmdMatch(2, inCmdArgs, "SET_NS")) {
            setNS(outLines, inLines[i]);
          } else {
            unknownCommand(outLines, inLines[i]);
          }
        }
        clock_t endTime = clock();
        double processTime = static_cast<double>(endTime - startTime)
            / CLOCKS_PER_SEC;
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
        double writeTime = static_cast<double>(endTime - startTime)
            / CLOCKS_PER_SEC;
        cout << "PROCESS LOOP FINISHED IN " << processTime << " seconds. Tool "
             << writeTime << " seconds to write output." << endl;
      } else {
        inLines.push_back(line);  // Otherwise store the line on the input buffer
      }
    } catch (std::exception& ex) {
      cout << "Exception: " << ex.what() << endl;
      break;
    }
  } while (!tstream->eof());
  try {
    cout << "closeing socket" << tstream->rdbuf()->remote_endpoint() << endl;
  } catch (std::exception& ex) {
    cout << "warning remote child socket closed prematurly: " << ex.what()
         << endl;
  }
  try {
    tstream->close();
  } catch (std::exception& ex) {
    cout << "exception: " << ex.what() << " while trying to close child socket"
         << endl;
  }
  return 0;
}

std::string GLBCommandServer::joinStr(const std::vector<std::string>& strIn,
                                      std::string delim) {
  return boost::algorithm::join(strIn, delim);
}

std::string GLBCommandServer::joinStr(const std::vector<std::string>& strIn,
                                      std::string delim, int start_i) {
  vector<string> strVec;
  int ss = strIn.size();
  for (int i = start_i; i < ss; i++) {
    strVec.push_back(strIn[i]);
  }
  return joinStr(strVec, " ");
}

int GLBCommandServer::splitStr(std::vector<std::string>& svOut,
                               std::string strIn, std::string delim) {
  boost::algorithm::replace_all(strIn, "\n", "");
  boost::algorithm::replace_all(strIn, "\r", "");
  boost::algorithm::split(svOut, strIn, boost::algorithm::is_any_of(delim),
                          boost::token_compress_on);
  return svOut.size();
}

void GLBCommandServer::start() {
  boost::thread th(bind(&GLBCommandServer::listener, this, m_ip_address, m_port));
  th.detach();
}

bool GLBCommandServer::cmdMatch(int nArgs, const std::vector<std::string>& sv,
                                std::string expected) {
  if (sv.size() >= nArgs && sv[0].compare(expected) == 0) {
    return true;
  }
  return false;
}

GLBCommandServer::~GLBCommandServer() {
  // TODO Auto-generated destructor stub
}

