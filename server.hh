#ifndef __SERVER_H
#define __SERVER_H

#include<string>
#include<boost/asio.hpp>
#include<boost/shared_ptr.hpp>
#include <pdns/ext/rapidjson/include/rapidjson/stringbuffer.h>
#include <pdns/ext/rapidjson/include/rapidjson/stringbuffer.h>
#include <pdns/ext/rapidjson/include/rapidjson/filestream.h>
#include <pdns/ext/rapidjson/include/rapidjson/writer.h>

#include "contrib/experiments/cpp/maindemo/rapidjson/document.h"
#include "contrib/experiments/cpp/maindemo/rapidjson/stringbuffer.h"

struct FqdnCount {
    std::string fqdn;
    long count;
};

class ServerJsonBuilder {
private:
    std::string error;
    bool hasError;
    std::string type;
    bool hasType;
    std::string fqdn;
    bool hasFqdn;
    std::string status;
    bool hasStatus;
    std::vector<FqdnCount> counts;
    bool hasCounts;
    std::vector<std::string> errors;
    bool hasErrors;
public:

    ServerJsonBuilder() {
        hasError = false;
        hasType = false;
        hasFqdn = false;
        hasErrors = false;
        hasCounts = false;
    }

    void setFqdn(std::string _fqdn) {
        fqdn = _fqdn;
        hasFqdn = true;
    }

    void setType(std::string _type) {
        type = _type;
        hasType = true;
    }

    void setError(std::string _error) {
        error = _error;
        hasError = true;
    }

    void addError(std::string errorEntry) {
        errors.push_back(errorEntry);
        hasErrors = true;
    }

    void setStatus(std::string _status) {
        status = _status;
        hasStatus = true;
    }

    void addCount(std::string fqdn, long count) {
        FqdnCount fc;
        fc.fqdn = fqdn;
        fc.count = count;
        counts.push_back(fc);
        hasCounts = true;
    }

    std::string to_json() {
        rapidjson::Document doc;
        rapidjson::Document::AllocatorType& aloc = doc.GetAllocator();
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> wr(sb);
        doc.SetObject();
        if (hasType) doc.AddMember("type", type.c_str(), aloc);
        if (hasFqdn) doc.AddMember("fqdn", fqdn.c_str(), aloc);
        if (hasStatus) doc.AddMember("status", status.c_str(), aloc);
        if (hasError) doc.AddMember("error", error.c_str(), aloc);
        if (hasCounts) {
            rapidjson::Value countArray;
            countArray.SetArray();
            std::vector<FqdnCount>::iterator it;
            for (it = counts.begin(); it != counts.end(); it++) {
                rapidjson::Value countElement;
                countElement.SetArray();
                rapidjson::Value countAsValue;
                countAsValue.SetArray();
                rapidjson::Value fqdnAsValue;
                fqdnAsValue.SetString((*it).fqdn.c_str());
                rapidjson::Value countJsonLong;
                countJsonLong.SetInt64((*it).count);
                countElement.PushBack(fqdnAsValue, aloc);
                countElement.PushBack(countJsonLong, aloc);
                countArray.PushBack(countElement, aloc);

            }
            doc.AddMember("counts", countArray, aloc);
        }
        if (hasErrors) {
            rapidjson::Value jsonArray;
            jsonArray.SetArray();
            std::vector<std::string>::iterator it;
            for (it = errors.begin(); it != errors.end(); it++) {
                rapidjson::Value errorMsg;
                errorMsg.SetString((*it).c_str());
                jsonArray.PushBack(errorMsg, aloc);
            }
            doc.AddMember("errors", jsonArray, aloc);
        }
        doc.Accept(wr);
        return std::string(sb.GetString());
    }
};

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
