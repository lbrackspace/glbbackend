#ifndef GLBCOMMANDSERVER_HH_
#define GLBCOMMANDSERVER_HH_

#include <string>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

class GLBCommandServer {
private:
    std::string m_ip_address;
    int m_port;
    void addDomain(std::vector<std::string>& outLines, std::string line);
    void delDomain(std::vector<std::string>& outLines, std::string line);
    void setSOA(std::vector<std::string> &outLines, std::string line);
    void setNS(std::vector<std::string> &outLines, std::string line);
    void debugDomains(std::vector<std::string>& outLines, std::string line);
    void unknownCommand(std::vector<std::string>&outLines, std::string line);
    bool decodeIP(const std::string inputStr, std::string &errorMsg, int &ipt,
            int &ttl, std::string &addr, std::string &attr);
    void snapshotDomain(std::vector<std::string> &outLines, std::string line);
    void getCounts(std::vector<std::string>& outLines, std::string line);
    void clearCounts(std::vector<std::string>& outLines, std::string line);
    int server(boost::shared_ptr<boost::asio::ip::tcp::iostream> tstream);
    bool cmdMatch(int nArgs, const std::vector<std::string>& sv,
            std::string expected);
    std::string joinStr(const std::vector<std::string>& strIn, std::string delim);
    std::string joinStr(const std::vector<std::string> &strIn, std::string delim,
            int start_i);
    int splitStr(std::vector<std::string>& svOut, std::string strIn,
            std::string delim);
    void clearDomains(std::vector<std::string>& svOut, std::string strIn);
public:
    GLBCommandServer(std::string ip_address, int port);
    void start();
    int listener();
    virtual ~GLBCommandServer();
};

#endif /* LISTENSERVER_HH_ */
