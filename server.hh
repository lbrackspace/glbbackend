#ifndef __SERVER_H
#define __SERVER_H

#include<string>
#include<boost/asio.hpp>
#include<boost/shared_ptr.hpp>

void start_server_thread(std::string ip_addr_str, int port);
bool cmdMatch(int nArgs, const std::vector<std::string>& sv, std::string expected);
void add_domain(std::vector<std::string>& outLines, std::string line);
void del_domain(std::vector<std::string>& outLines, std::string line);
void set_soa(std::vector<std::string> &outLines, std::string line);
void set_ns(std::vector<std::string> &outLines, std::string line);
void debug_domains(std::vector<std::string>& outLines, std::string line);
void unknown_command(std::vector<std::string>&outLines, std::string line);
bool decodeIP(const std::string inputStr, std::string &errorMsg, int &ipt, int &ttl, std::string &addr, std::string &attr);
void snapshot_domain(std::vector<std::string> &outLines, std::string line);
void get_counts(std::vector<std::string>& outLines, std::string line);
void clr_counts(std::vector<std::string>& outLines, std::string line);
int usage(char *prog);
int listener(std::string ip_addr_str, int port);
int server(boost::shared_ptr<boost::asio::ip::tcp::iostream> tstream);
std::string joinStr(const std::vector<std::string>& strIn, std::string delim);
std::string joinStr(const std::vector<std::string> &strIn, std::string delim, int start_i);
int splitStr(std::vector<std::string>& svOut, std::string strIn, std::string delim);
#endif
