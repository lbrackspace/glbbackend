#ifndef IPRECORD_H
#define	IPRECORD_H

#include<sstream>
#include<string>
#include<boost/unordered_map.hpp>

class IPRecord {
private:
    int ipType;
    std::string ipAddress;
    int ttl;

public:

    IPRecord(int ipt, std::string ipa, int tl) {
        ipAddress = ipa;
        ttl = tl;
        ipType = ipt;
    }

    IPRecord() {
    }

    IPRecord(const IPRecord& orig) {
        this->ipType = orig.ipType;
        this->ttl = orig.ttl;
        this->ipAddress = orig.ipAddress;
    }

    virtual ~IPRecord() {
    }

    std::string getIPAddress() const {
        return ipAddress;
    }

    int getTtl() const {
        return ttl;
    }

    void setTtl(int tl) {
        ttl = tl;
    }

    void setIPAddress(std::string ipa) {
        ipAddress = ipa;
    }

    std::string to_string();
};

int strToIpType(std::string str);
std::string ipTypeToStr(int ipt);

#endif	/* IPRECORD_H */

